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
//

#ifndef __SINK_H
#define __SINK_H

#include <omnetpp.h>

// we use this to track each message
class MsgTupple
{
public:
    unsigned int srcId;
    unsigned int appIdx;
    unsigned int msgIdx;
    MsgTupple(unsigned int s, unsigned int a, unsigned int m)
    {
        srcId = s;
        appIdx = a;
        msgIdx = m;
    };
    std::string dump() const
    {
        char buff[128];
        sprintf(buff, " src: %d app: %d msg: %d", srcId, appIdx, msgIdx);
        return (std::string(buff));
    };
    bool operator()(const MsgTupple &a, const MsgTupple &b) const
    {
        return ((a.srcId < b.srcId) || ((a.srcId == b.srcId) && (a.appIdx < b.appIdx)) || ((a.srcId == b.srcId) && (a.appIdx == b.appIdx) && (a.msgIdx < b.msgIdx)));
    }
    bool operator==(const MsgTupple &b) const
    {
        return ((this->srcId == b.srcId) && (this->appIdx == b.appIdx) && (this->msgIdx == b.msgIdx));
    }
    bool operator<(const MsgTupple &b) const
    {
        return ((this->srcId < b.srcId) ||
                (this->srcId == b.srcId && (this->appIdx < b.appIdx)) ||
                (this->srcId == b.srcId && (this->appIdx == b.appIdx) && (this->msgIdx < b.msgIdx)));
    }
};

class MsgTuppleLess
{
public:
    bool operator()(const MsgTupple &a, const MsgTupple &b)
    {
        return ((a.srcId < b.srcId) ||
                ((a.srcId == b.srcId) && (a.appIdx < b.appIdx)) ||
                ((a.srcId == b.srcId) && (a.appIdx == b.appIdx) && (a.msgIdx < b.msgIdx)));
    }
};

// store msg context
class OutstandingMsgData
{
public:
    omnetpp::simtime_t firstFlitTime;
    omnetpp::simtime_t enoughPktsLastFlitTime;
    unsigned int numPktsReceived;

    OutstandingMsgData() { numPktsReceived = 0; };
};

//
// Consumes IB Credits; see NED file for more info.
//
class IBSink : public omnetpp::cSimpleModule
{
private:
    omnetpp::cMessage *p_hiccupMsg;
    omnetpp::cMessage *p_drainMsg;

    // parameters
    double popDlyPerByte_ns;      // the PCI Exp drain rate per byte
    int maxVL;                    // Maximum value of VL
    double flitSize;              // size in bytes of single flit
    double startStatCol_sec;      // time to start co
    unsigned int repFirstPackets; // if not zero declare a message as done after first repFirstPackets arrived
    unsigned packet_counter_ = 0;

    // data structure
    int duringHiccup; // set to 1 if during a hiccup
    omnetpp::cQueue queue;
    omnetpp::simtime_t lastConsumedPakcet;                // the last time a packet was consumed
    unsigned int lid;                                     // the HCA LID
    std::map<unsigned int, unsigned int> lastPktSnPerSrc; // last packet serial number per SRC
    // in order to calculate the message latencies we track outstanding messages,
    // first pkt arrival and number of rec packets
    // std::map<MsgTupple, class OutstandingMsgData, MsgTuppleLess> outstandingMsgsData;
    std::map<MsgTupple, class OutstandingMsgData /*, MsgTuppleLess*/> outstandingMsgsData;

    // methods
    void newDrainMessage(double delay);
    void consumeDataMsg(IBDataMsg *p_msg);
    void handlePop(omnetpp::cMessage *p_msg);
    void handleData(IBDataMsg *p_msg);
    void handleHiccup(omnetpp::cMessage *p_msg);
    void handleSinkTimer(IBSinkTimerMsg *p_msg);

    // statistics
    omnetpp::cDoubleHistogram PakcetFabricTime;
    omnetpp::cStdDev waitStats;                  // Data Packets Wait Time statistics
    omnetpp::cStdDev hiccupStats;                // statistics about hiccups
    std::vector<int> VlFlits;                    // total number of FLITs per VL
    int AccBytesRcv;                             // total number of bytes received
    omnetpp::cOutVector oooPackets;              // vector of number of total OOO packets received
    unsigned int totOOOPackets;                  // total number of OOO packets received
    omnetpp::cStdDev oooWindow;                  // in packets
    unsigned int totOOPackets;                   // the total number of packets that need retransmission inc the window
    unsigned int totIOPackets;                   // the total packets received in order
    omnetpp::cDoubleHistogram msgLatency;        // the network latency of received messages from the
                                                 // time first msg flit was injected to the time last msg flit received
    omnetpp::cDoubleHistogram msgF2FLatency;     // the network latency of received messages from the
                                                 // time first msg flit was injected to the time last packet first flit received
    omnetpp::cDoubleHistogram enoughPktsLatency; // the network latency of received repFirstPackets of the messages
                                                 // from the time first msg flit was injected to the time the last
                                                 // flit of the first repFirstPackets was received
    omnetpp::cStdDev enoughToLastPktLatencyStat; // statistics about the time difference from enough pkts to last pkt

    // for IBA congestion control
    omnetpp::cOutVector flags; // track the flags (FECN, BECN) over time
    // IBPushFECNMsg *p_pushFECNmsg;

    omnetpp::cOutVector latency; // track the flags (FECN, BECN) over time
    omnetpp::cOutVector largelatency;

    int on_cc;
    int on_newcc;
    int BECNRecv;
    std::vector<int> FECNRecv;
    // std::vector<int> VictimRecv;
    std::vector<int> Recv;
    std::vector<int> temp;
    omnetpp::simtime_t PktRecvTime;               // track the rtt
    std::vector<omnetpp::simtime_t> FECNRecvTime; // track the fecn receive time
    omnetpp::cOutVector FECNRecvPackets;          // vector of number of fecn received
    omnetpp::cOutVector VictimRecvPackets;        // vector of number of victim packets
    double RecvRate;
    omnetpp::cOutVector RecvRateRecord;         // vector of received rate
    std::vector<IBSinkTimerMsg *> sinktimerMsg; // the self push message
    double periodT;
    std::vector<omnetpp::simtime_t> FirstRecvTime; // track the first packet receive time
    int Recv_throughput;
    omnetpp::cOutVector throughput;
    double timeStep_us;
    int on_utilization;
    omnetpp::simtime_t LastRecvTime; // track the rtt
    omnetpp::simtime_t FirstSendCNPTime;
    omnetpp::simtime_t SendCNPTime;
    long cnpsent;

protected:
    virtual void initialize();
    virtual void handleMessage(omnetpp::cMessage *msg);
    virtual void finish();
    virtual ~IBSink();
};

#endif
