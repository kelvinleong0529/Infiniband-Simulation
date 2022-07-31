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
#ifndef __IB_MODEL_AR_PKTFWD_H_
#define __IB_MODEL_AR_PKTFWD_H_

#include <omnetpp.h>

//
// The packet forwarder is responsible for output port selection
//
class Pktfwd : public omnetpp::cSimpleModule
{
public:

  // parameters
  double statRepTime_s; // time between statistics reporting

  // state
  int numPorts;          // number of switch ports
  std::vector<int> *FDB; // deterministic routing out port by dlid from vec file
  cModule* Switch;

public:
  // get the output port for the given LID
  virtual int getPortByLID(unsigned int sLid, unsigned int dLid);
  
  // report queuing of flits on TQ for DLID (can be negative for arb)
  virtual int repQueuedFlits(unsigned int rq, unsigned int tq, unsigned int dlid, int numFlits);

  // handle a report about port loading and unloading
  virtual void handleTQLoadMsg(unsigned int tq, unsigned int srcRank, unsigned int fitstLid, unsigned int lastLid, int load);

protected:
    virtual void initialize();
    virtual void finish();
	 ~Pktfwd();
};

#endif
