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
// IB Device Port Input Buffer
// ============================
// Ports:
// * in     - where data arrives into the buffer
// * out[]  - through which they leave to each target out port VLArb
// * sent[] - gets message from the VLArb to inform HOQ completed transmission
// * rxCred - forward ABR+FREE of the local buffer to the OBUF
// * txCred - provide update of FCCL from received flow control to the
//            VLA of this port
// * done   - When done sending a packet provide signal to all the VLAs
//            connected to the IBUF about the change in number of busy ports.
//
// Parameters:
// MaxStatic[vl]  - max static credits allocated for each VL
// BufferSize     - the input buffer size in bytes
//
// External Events:
// * push - data is available on the input (either flow control or credit)
// * sent - the hoq was sent from the VLArb
//
// Functionality:
// * On Push the packet is classified to be flow control or data:
//   If it is flow control info:
//     The FCCL is delivered through the TxCred to the VLA
//     ARB is overwritten with FCTBS (and cause a RxCred send to OBUF)
//   If it is DATA Packet:
//     If the first credit of the packet need to decide which buffers
//     should be used for storing the packet:
//     - Lookup the target port of this pakcet by consulting the LFT
//       track it in CurOutPort
//     - Make sure enough credits exists for this VL (inspecting the
//       FREE[VL]). If there are not enough credits ASSERT.
//       FREE[VL] = MaxStatic[VL] - UsedStatic[VL]
//
//   For every DATA "credit" (not only first one)
//     - Queue the Data in the Q[V]
//          staticFree[VL]++
//          ABR[VL]++
//     - Send RxCred with updated ARB[VL] and FREE[VL] - only if sum changed
//       which becomes the FCCL of the sent flow control
//     - If HoQ in the target VLA is empty - send the push event out.
//       when the last packet is sent the "done" event is sent to all output
//       ports
//
// * On Sent:
//   - Decrease UsedStatic[VL] and update FREE[VL]
//   - If the message that was sent is the last in the packet we need to
//     send the "done" message to all connected ports.
// 
// WHEN DO WE SEND DATA OUT?
// * At any given time not more the maxBeingSent packets can be sent.
//   The VLAs track this. Each VLA that wants to send the HOQ
//   looks at the numBeingSent and if < maxBeingSent increases it.
//   If not it can not send.
// * When a new packet is received an output port should be looked up (LFT)
// * If HOQ in the VLA is empty - send a push to the VLA.
// * When VLA completes sending the credit it provides back the "sent". Then
//   a new credit is moved to the HOQ in the VLA.
// * On the last credit of sent packet we decreas the numBusyPorts. Send "done"
//   to all the connected VLAs
//
//

#ifndef __IBUF_H
#define __IBUF_H

#include <omnetpp.h>
#include <map>
#include <vector>
#include "pktfwd.h"
#define MAX_LIDS 10

// Store packet specific information to store the packet state  
class PacketState {
  int outPort; // the out port

 public:
  PacketState(int pn) {
    outPort = pn;
  };
};

//
// Input Buffer for Receiving IB FLITs and VL credit updates
//
class IBInBuf : public omnetpp::cSimpleModule
{
 private:
   omnetpp::cMessage *p_popMsg;
  omnetpp::cMessage *p_minTimeMsg;

  // parameters:
  double ISWDelay ; // delay in ns contributed by SW in IBUF
  std::vector<unsigned int> maxStatic; // max static credits for each VL
  int maxBeingSent;             // max num packets sent out at a given time
  unsigned int totalBufferSize; // The total buffer size in credits
  unsigned int numPorts;        // the number of ports we drive
  unsigned int maxVL;           // Maximum value of VL
  unsigned int width;           // the width of the incoming port 1/4/8/12
  int hcaIBuf;                  // > 0 if an HCA port IBuf
  bool lossyMode;               // if true make this port lossy

  // data strcture
  int numBeingSent;   // Number of packets being currently sent
  
  int hoqOutPort[8];  // The output port the packet at the HOQ is targetted to
  std::vector<unsigned int> staticFree;  // number of free credits per VL
  std::vector<long> ABR;    // total number of received credits per VL
  unsigned int thisPortNum; // holds the port num this is part of

  // there is only one packet stream allowed on the input so we track its
  // parameters simply by having the "current" values. We check for mix on the
  // push handler
  int curPacketId; 
  int curPacketSrcLid;
  std::string curPacketName;
  unsigned int curPacketCredits;
  int curPacketVL;
  int curPacketOutPort;
  omnetpp::simtime_t lastSendTime;

  // as we might have multiple sends we need to track the "active sends"
  // given the packet ID we track various state variables.
  std::map<int, PacketState, std::less<int> > activeSendPackets;

  // pointer the container switch
  omnetpp::cModule* Switch;
  Pktfwd* pktfwd;
  int portsnum_parent;

  // statistics
  omnetpp::cLongHistogram staticUsageHist[8];
  omnetpp::cOutVector usedStaticCredits;
  omnetpp::cOutVector CredChosenPort;
  omnetpp::cOutVector dsLidDR;
  omnetpp::cOutVector outPortDR;
  omnetpp::cOutVector pktidDR;
  unsigned int numDroppedCredits;
  omnetpp::cOutVector inputQueueLength;
  omnetpp::cOutVector CreditLimit;

  int marknum;
  int BECNRecv;
  int FECNRecv;
  int markrate;

  // methods
  long getDoneMsgId();
  void parseIntListParam(char *parName, int numEntries, std::vector<int> &out);
  void sendOutMessage(IBWireMsg *p_msg);
  void qMessage(IBWireMsg *p_msg);
  void handleSent(IBSentMsg *p_msg);
  void sendRxCred(int vl, double delay); // send a RxCred message to the OBUF
  void sendTxCred(int vl, long FCCS); // send a TxCred message to the VLA
  void updateVLAHoQ(short int portNum, short vl); // send the HoQ if you can
  void simpleCredFree(int vl); // perform a simple credit free flow

  // return 1 if the HoQ at the given port and VL is free
  int isHoqFree(int portNum, int vl);
  void handlePush(IBWireMsg *p_msg);
  void handleTQLoadMsg(IBTQLoadUpdateMsg *p_msg);
  virtual void initialize();
  virtual void handleMessage(omnetpp::cMessage *msg);
  virtual void finish();
  virtual ~IBInBuf();

 public:

  omnetpp::cQueue **Q;         // Incoming packets Q per VL per out port
  // return 1 if incremented the number of parallel sends
  int incrBusyUsedPorts();

  
};

#endif
