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
// An IB Packet Generator - Injecting Credits into the "out" port
//
// Overview:
// =========
// The task of the IB packet generator is to push FLITs (64 bytes) into
// the network. It represents the scheduer that decides which "flow" will
// be served by the HCA.
//
// The requests arrives on the in port and are saved per application.
// At any given time there is one appMsg that is being served and it
// continue sending out unless the VL buffer for it fills in or maxContPkts
// is reached. When it is done the scheduler Round Robin on the messages.
//
// TBD: support QoS
//
// Packet generation can be configured with the following set of orthogonal
// mechanisms:
// * Injection rate and burstiness - we incorporate inter/intra packet
//   and inter/intra credit latencies
// * SQ2VL - what VL the packet will traverse with
//
// The mechanisms above are all orthogonal so the user needs to select
// a combination of each mechanism to fully configure the generator
//
// Events:
// =======
// New FLITs are injected by the generator on "push" event.
// When a new packet is inserted into a VL Q it will be immediatly sent if the
// VLA HoQ is empty. Otherwise sending of the packets to the VLA hardware
// on the receiving the "sent" event.
// AppMsg events are input on the "in" port. When an AppMsg is completed
// it is sent back to the App therough the in port.
//
// App Selection:
// ======================
// A round robin arbiter is selecting which app to serve. The arbiter
// known what VLQs are full and skip messages that map to it.
// It will serve max of maxContPkts of the current app.
//
// Parameters for destination selection:
// maxContPkts - maximal number of packets of single app to send
// maxQueuedPerVL - the maximal outstanding FLITs in Q per VL
//
// Packet Size Selection:
// ==============================
// Parameters that control packet size:
// flitSize_B - how many byted in single FLIT
// NOTE: due to current limitation of the simulator of sending full flits
// all sizes are padded to flitSize ...
//
// Traffic Shaping:
// ================
// Parameters that control shaping:
// flit2FlitGap_ns - the extra delay from one flit end to the other start [ns]
// pkt2PktGap_ns - the extra delay from one packet end to the next start [ns]
//
// SQ/VL selection:
// ================
// Currently there is one to one mapping
//
// parameters
// maxVL - the value of the maximal VL
//
// Other Parameters:
// =================
// genDlyPerByte_ns - the time it takes the gen to generate a new FLIT
// popDlyPerByte_ns - the time it takes to push generate a FLIT to VLA
//

#ifndef __GEN_H
#define __GEN_H

#include <omnetpp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//
// Generates IB Packet Credit (messages); see NED file for more info.
//
class IBGenerator : public omnetpp::cSimpleModule
{
private:
  // parameters:
  unsigned int srcLid;         // the generator LID
  double flitSize_B;           // number of bytes in each credit/FLIT
  unsigned int maxContPkts;    // maximal continoues packets for msg
  unsigned int maxQueuedPerVL; // maximal num FLITs Queued on VL Q
  double genDlyPerByte_ns;     // the time it takes to bring Byte from PCIe

  // - shape
  double flit2FlitGap_ns; // extra delay from one flit end to the other
  double pkt2PktGap_ns;   // extra delay from one packet end to the next

  // - VL
  unsigned int maxVL;

  // state
  unsigned int msgIdx;                                  // count number of messages injected
  unsigned int curApp;                                  // currently surved app
  unsigned int numApps;                                 // width of the in port
  unsigned int numContPkts;                             // count the number of packets of same app
  std::vector<IBAppMsg *> appMsgs;                      // requested messages by app port
  std::vector<omnetpp::cQueue> VLQ;                     // holds outstanding out packets if any
  unsigned int pktId;                                   // packets counter
  omnetpp::cMessage *pushMsg;                           // the self push message
  std::map<unsigned int, unsigned int> lastPktSnPerDst; // last packet serial number per DST

  // statistics
  omnetpp::simtime_t firstPktSendTime; // the first send time
  omnetpp::simtime_t lastPktSendTime;  // the last send time
  unsigned int totalBytesSent;         // total number of bytes sent
  omnetpp::simtime_t timeLastSent;     // Time last flit was sent

  // for throughput statistics
  int on_throughput_gen;
  int on_average_throughput;
  omnetpp::cMessage *timerMsg;       // the self push message
  omnetpp::simtime_t timeLastPeriod; // last record time
  unsigned int BytesSentLastPeriod;  // total number of bytes sent last period
  double timeStep_us;                // total number of bytes sent last period
  omnetpp::cOutVector throughput;    // track the throughput over time
  omnetpp::simtime_t PktSendTime;    // track the rtt
  double startTime_s;
  double endTime_s;

  // for IBA congestion control
  int on_cc;
  int on_newcc;
  std::vector<double> CCT_Index; // CCT_Index by app port
  int CCT_Limit;
  int CCT_MIN;
  unsigned int increaseStep_us;
  omnetpp::cMessage *cctimerMsg; // the self push message for cc timer
  omnetpp::cOutVector gap;
  double send_interval_ns;
  omnetpp::cMessage *sendtimerMsg; // the self push message for cc timer
  double last_RecvRate;
  double target;
  int last_BECNValue;
  int last_BECNValue_count;
  double send_interval_ns_last;

  // for statistics
  int gen_BECN;
  int sent_BECN;
  omnetpp::cOutVector BECN_msgIdx; // track the  over time
  omnetpp::cOutVector cct_index;   // track the cct_index over time

  std::vector<omnetpp::simtime_t> Last_BECN; // CCT_Index by app port
  std::string output;
  FILE *fd;

  omnetpp::simtime_t LastPktSendTime; // last packet

  // methods
private:
  // Create a new FLIT for the current Packet
  IBDataMsg *getNewDataMsg();

  bool arbitrateApps();
  void getNextAppMsg();
  void genNextAppFLIT();
  void initPacketParams(IBAppMsg *p_msg, unsigned int pktIdx);
  unsigned int vlBySQ(unsigned sq);
  int isRemoteHoQFree(int vl);
  void sendDataOut(IBDataMsg *p_msg);
  void handlePush(omnetpp::cMessage *msg);
  void handleTimer(omnetpp::cMessage *msg);
  void handlePushFECN(IBPushFECNMsg *msg); // receive FECN from sink module
  void handlePushBECN(IBPushBECNMsg *msg); // receive BECN from sink module
  void handleSent(IBSentMsg *p_sent);
  void handleApp(IBAppMsg *p_msg);
  void handleSendTimer(omnetpp::cMessage *msg); // timer for send packets. Previously the send of packets are controlled by flow control
  virtual ~IBGenerator();
  void incrementApp(int appidx);

protected:
  virtual void initialize();
  virtual void handleMessage(omnetpp::cMessage *msg);
  virtual void finish();
};

#endif
