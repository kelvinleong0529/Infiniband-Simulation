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
// IB Device Port Output Buffer
// ============================
// Ports
// * in[N]
// * out 
// * txCred - where flow control data is through
// * sent[N] - backward notification of sent data
//
// Parameters
// * VLHighLimit - IB style VLArb table limit of number of credits sent
//                 from high level before low level checked.
// * HighVLs[C], HighWeights[C], LowVLs[C], LowWeights[C] - the IB VLA config
// * popDelayPerByte_s - control the rate of Pop events
// 
// Internal Events
// * Pop - cause a credit to leave the 
// 
// External Events
// * Push - data is available on the INj (stored locally)
// * txCred - FCCL = credits availability on remote port
//
// Functionality
// Every time a packet can be sent out as dictated by the Pop rate performs
// IB style VLArb by inspecting available credits and packets.
// Use round robin to select input port if data is availble on several.
// Track sent packets on each VL (FCTBS). The txCred event updates the FCCL[vl]
// Free credits per VL are calculated as FCCL - FCTBS. 
// When a credit is sent out of the VLArb a "Sent" event is provided back to
// the IBUF (such that it can free buffers).
//
// Data Structure
// To avoid duplicating and multiply by the number of ports the IBUF only 
// keeps one credit message per input port, per VL. 
// The IBUF should push its available credit immediaty to the VLArb
// The VLArb notify the IBUF that packet has left and the buffer is
// not empty using the Sent message  
// 
//
#ifndef __VLARB_H
#define __VLARB_H

#include <omnetpp.h>
#include <vector>

//
// A single entry in the arbitration table
// 
class ArbTableEntry {
public:
  short int VL;
  short int weight; // max credits to send - rounded up for a packet
  int       used;   // used credits from the above
};

//
// Infiniband Arbiter
//
class IBVLArb: public omnetpp::cSimpleModule
{
 private:
  omnetpp::cMessage *p_popMsg;
  
  // parameters:
  int vlHighLimit;          // Max number of credits sent from High till Low
  ArbTableEntry HighTbl[8]; // The High Priority Arbitration Table
  ArbTableEntry LowTbl[8];  // The Low Priority Arbitration Table
  int           hcaArb;     // If 1 means the arbiter is an HCA arbiter
  //int connectwithhca;
  int recordVectors;        // Control recording of vectors
  unsigned int maxVL;       // Maximum value of VL
  double VSWDelay;             // Delay brought by VLArb in Switch [ns] (SW par)
  bool useFCFSRQArb;        // Arbitrate RQs on same VL by First Come First Serve

  // data strcture:
  double popDelayPerByte_s;  // Rate of single byte injection 
  IBDataMsg ***inPktHoqPerVL; // the head of the send Q on every VL
  short **hoqFreeProvided;    // set when a "free" HoQ provided/cleared on push
  unsigned int HighIndex, LowIndex; // points to the index in the VLArb tables.
  std::vector<int> LastSentPort; // last port that have sent data on each VL
  unsigned int LastSentVL; // the VL of the last sent packet
  int SentHighCounter;     // counts how many credits were sent from the high
  int LastSentWasHigh;     // 1 if we were previously sending from high
  int LastSentWasLast;     // 1 if the sent data was last in the packet
  unsigned int numInPorts; // The number of input ports
  int InsidePacket;        // if 1 we are sending a packet (already arbitrated)
  omnetpp::simtime_t lastSendTime;  // store the last arbitration send

  // methods
  void setVLArbParams(const char *cfgStr, ArbTableEntry *tbl);
  void sendOutMessage(IBDataMsg *p_msg);
  void sendSentMessage(unsigned int portNum, unsigned int vl);
  int  isValidArbitration(unsigned int portNum, unsigned int vl,
				  int isFirstPacket, int numPacketCredits);
  int roundRobinNextRQForVL(int numCredits, unsigned int curPortNum, short int vl,
							int &nextPortNum);
  int firstComeFirstServeNextRQForVL(int numCredits, unsigned int curPortNum, short int vl,
  							   int &nextPortNum);
  int  findNextSend( unsigned int &curIdx, ArbTableEntry *Tbl, 
			   unsigned int &curPortNum, unsigned int &curVl );
  int  findNextSendOnVL0( unsigned int &curPortNum );
  void displayState();
  void arbitrate();
  void handlePush(IBDataMsg *p_msg);
  void handlePop();
  void handleTxCred(IBTxCredMsg *p_msg);
  int  getOBufFCTBS(unsigned int vl);

  // statistics
  omnetpp::cOutVector vl0Credits;  // the credits on VL0
  omnetpp::cOutVector vl1Credits;  // the credits on VL1
  omnetpp::cOutVector readyData;   // the VLs with ready data in binaru code
  omnetpp::cOutVector arbDecision; // the resulting VL arbitrated -1 is invalid
  omnetpp::cLongHistogram portXmitWaitHist;
  int sent_BECN;
  omnetpp::cOutVector StaticFreeNum;
  omnetpp::cOutVector CreditsNum;
  omnetpp::cOutVector MarkTime;
  int markrate;
  int markcount;
  int NewCreditsNum;
  int FullCredit;

 public:
  // return 1 if the HoQ for that port/VL is free
  int isHoQFree(unsigned int pn, unsigned int vl); 

  // number of data packet credits sent total in this VL
  std::vector<long> FCTBS;
  
  // The last number of credits the receive port provided 
  std::vector<long> FCCL; 

 protected:
  virtual void initialize();
  virtual void handleMessage(omnetpp::cMessage *msg);
  virtual void finish();
  ~IBVLArb();
};

#endif

