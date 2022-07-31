///////////////////////////////////////////////////////////////////////////
//
//         InfiniBand FLIT (Credit) Level OMNet++ Simulation Model
//
// Copyright (c) 2004-2013 Mellanox Technologies, Ltd. All rights reserved.
// This software is available to you under the terms of the GNU
// General Public License (GPL) Version 2, available from the file
// COPYING in the main directory of this source tree.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////
//
// IB App Message Generator:
//
// Internal Messages:
// None
//
// External Messages:
// done/any - IN: the last provided appMsg was consumed - gen a new one
// IBAppMsg - OUT: send the message to the gen
//
// For a full description read the app.h
//

#include "ib_m.h"
#include "app.h"
#include "vlarb.h"
#include "vec_file.h"
using namespace std;

Define_Module(IBApp);

void IBApp::parseIntListParam(const char *parName, std::vector<int> &out)
{
  int cnt = 0;
  const char *str = par(parName);
  char *tmpBuf = new char[strlen(str) + 1];
  strcpy(tmpBuf, str);
  char *entStr = strtok(tmpBuf, " ,");
  if (out.size())
    out.clear();
  while (entStr)
  {
    cnt++;
    out.push_back(atoi(entStr));
    entStr = strtok(NULL, " ,");
  }
}

// main init of the module
void IBApp::initialize()
{
  // init destination sequence related params
  dstSeqIdx = 0;
  dstSeqDone = 0;
  msgIdx = 0;
  msgNum = par("msgNum");
  startTime_s = par("startTime");
  endTime_s = par("endTime");

  //  destination mode
  const char *dstModePar = par("dstMode");
  if (!strcmp(dstModePar, "param"))
  {
    msgDstMode = DST_PARAM;
  }
  else if (!strcmp(dstModePar, "seq_once"))
  {
    msgDstMode = DST_SEQ_ONCE;
  }
  else if (!strcmp(dstModePar, "seq_loop"))
  {
    msgDstMode = DST_SEQ_LOOP;
  }
  else if (!strcmp(dstModePar, "seq_rand"))
  {
    msgDstMode = DST_SEQ_RAND;
  }
  else
  {
    error("unknown dstMode: %s", dstModePar);
  }

  // destination related parameters
  if (msgDstMode != DST_PARAM)
  {
    const char *dstSeqVecFile = par("dstSeqVecFile");
    const int dstSeqVecIdx = par("dstSeqVecIdx");
    vecFiles *vecMgr = vecFiles::get();
    dstSeq = vecMgr->getIntVec(dstSeqVecFile, dstSeqVecIdx);
    if (dstSeq == NULL)
    {
      // opp_error("fail to obtain dstSeq vector: %s/%d", dstSeqVecFile, dstSeqVecIdx);
    }
    EV << "-I- Defined DST sequence of " << dstSeq->size() << " LIDs" << endl;
    delete vecMgr;
  }

  // Message Length Modes
  const char *msgLenModePar = par("msgLenMode");
  if (!strcmp(msgLenModePar, "param"))
  {
    msgLenMode = MSG_LEN_PARAM;
  }
  else if (!strcmp(msgLenModePar, "set"))
  {
    msgLenMode = MSG_LEN_SET;
  }
  else
  {
    // opp_error("unknown msgLenMode: %s", msgLenMode);
  }

  // need to init the set...
  if (msgLenMode == MSG_LEN_SET)
  {
    parseIntListParam("msgLenSet", msgLenSet);
    vector<int> msgLenProbVec;
    parseIntListParam("msgLenProb", msgLenProbVec);

    if (msgLenSet.size() != msgLenProbVec.size())
    {
      error("provided msgLenSet size: %d != msgLenProb size: %d",
            msgLenSet.size(), msgLenProbVec.size());
    }

    // convert the given probabilities into a histogram
    // with Prob[idx] where idx is the index of the length in the vector
    msgLenProb.setNumCells(msgLenSet.size());
    msgLenProb.setRange(0, msgLenSet.size() - 1);
    // HACK: there must be a faster way to do this!
    for (unsigned int i = 0; i < msgLenProbVec.size(); i++)
      for (int p = 0; p < msgLenProbVec.at(i); p++)
        msgLenProb.collect(i);

    EV << "-I- Defined Length Set of " << msgLenSet.size() << " size" << endl;
  }

  seqIdxVec.setName("Dst-Sequence-Index");

  // if we are in param mode we may be getting a 0 as DST and thus keep quite
  if (msgDstMode == DST_PARAM)
  {
    int dstLid = par("dstLid");
    if (dstLid)
    {
      scheduleAt(omnetpp::simTime() + startTime_s, new omnetpp::cMessage);
    }
  }
  else
  {
    // Emulate a "done"
    scheduleAt(omnetpp::simTime(), new omnetpp::cMessage);
  }
  msgInfo.setName("msgInfo");
}

// get random msg len by the histogram
unsigned int IBApp::getMsgLenByDistribution()
{
  double r = msgLenProb.random();
  return int(r);
}

// Initialize the parameters for a new message by sampling the
// relevant parameters and  allocate and init a new message
IBAppMsg *IBApp::getNewMsg()
{

  unsigned int msgMtuLen_B; // MTU of packet. same for entire message.
  unsigned int msgLen_P;    // the message length in packets
  unsigned int msgLen_B;    // the length of a message in bytes
  unsigned int msgSQ;       // the SQ to be used
  unsigned int msgDstLid;   // destination lid

  msgMtuLen_B = par("msgMtuLen");
  msgSQ = par("msgSQ");

  // obtain the message length
  switch (msgLenMode)
  {
  case MSG_LEN_PARAM:
    msgLen_B = par("msgLength");
    break;
  case MSG_LEN_SET:
    msgLen_B = getMsgLenByDistribution();
    break;
  default:
    error("unsupported msgLenMode: %d", msgLenMode);
    break;
  }

  msgLen_P = msgLen_B / msgMtuLen_B;

  // obtain the message destination
  switch (msgDstMode)
  {
  case DST_PARAM:
    msgDstLid = par("dstLid");
    break;
  case DST_SEQ_ONCE:
    msgDstLid = (*dstSeq)[dstSeqIdx++];
    if (dstSeqIdx == dstSeq->size())
    {
      dstSeqDone = 1;
    }
    seqIdxVec.record(dstSeqIdx);
    break;
  case DST_SEQ_LOOP:
    msgDstLid = (*dstSeq)[dstSeqIdx++];
    if (dstSeqIdx == dstSeq->size())
    {
      dstSeqIdx = 0;
    }
    seqIdxVec.record(dstSeqIdx);
    break;
  case DST_SEQ_RAND:
    dstSeqIdx = intuniform(0, dstSeq->size() - 1);
    msgDstLid = (*dstSeq)[dstSeqIdx];
    break;
  default:
    error("unsupported msgDstMode: %d", msgDstMode);
    break;
  }

  IBAppMsg *p_msg;
  char name[128];
  sprintf(name, "app-%s-%d", getFullPath().c_str(), msgIdx);
  p_msg = new IBAppMsg(name, IB_APP_MSG);
  p_msg->setAppIdx(getIndex());
  p_msg->setMsgIdx(msgIdx);
  p_msg->setDstLid(msgDstLid);
  p_msg->setSQ(msgSQ);
  p_msg->setLenBytes(msgLen_B);
  p_msg->setLenPkts(msgLen_P);
  p_msg->setMtuBytes(msgMtuLen_B);
  msgIdx++;
  return p_msg;
}

void IBApp::handleMessage(omnetpp::cMessage *p_msg)
{
  delete p_msg;
  double delay_ns = par("msg2msgGap");

  // if (!dstSeqDone && omnetpp::simTime() < endTime_s ) {
  if (!dstSeqDone /*&& omnetpp::simTime() + delay_ns*1e-9 < endTime_s*/ && msgIdx < msgNum)
  {
    // if (!dstSeqDone && omnetpp::simTime() + 1024*1e-9 < endTime_s /*&& msgIdx < msgNum*/) {
    //  generate a new messaeg and send after hiccup
    IBAppMsg *p_new = getNewMsg();
    // msgInfo.record(p_new->getMsgIdx());

    sendDelayed(p_new, delay_ns * 1e-9, "out$o");

    EV << "-I- " << getFullPath()
       << " sending new app message " << p_new->getName()
       << endl;
  }
}

void IBApp::finish()
{
  ;
}

IBApp::~IBApp()
{
  return;
}
