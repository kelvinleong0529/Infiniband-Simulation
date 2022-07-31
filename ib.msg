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

// Enumarate all IB massages
enum IB_MSGS
{
  IB_DATA_MSG     = 1; 
  IB_FLOWCTRL_MSG = 2; 
  IB_SENT_MSG     = 3; 
  IB_TXCRED_MSG   = 4; 
  IB_RXCRED_MSG   = 5; 
  IB_MINTIME_MSG  = 6; 
  IB_POP_MSG      = 7; 
  IB_HICCUP_MSG   = 8;
  IB_FREE_MSG     = 9;
  IB_DONE_MSG     = 10;
  IB_APP_MSG      = 11;
  IB_PUSH_MSG     = 12;
  IB_TQ_LOAD_MSG  = 13; 
  IB_TIMER_MSG  = 14;     // general timer, used for throughput calculation etc
  IB_PUSHFECN_MSG  = 15;
  IB_PUSHBECN_MSG  = 16;
  IB_CCTIMER_MSG  = 17;   // IB CC timer for rate increase
  IB_SENDTIMER_MSG  = 18;
  IB_SINKTIMER_MSG = 19;  // timer at sink module for new cc control
}

///////////////////////////////////////////////////////////////////////////
//
// MESSAGES THAT ARE AVAILABLE ON IB WIRE
//

//
// Base class for all type of IB wire messages
//

message IBWireMsg extends cPacket
{
  short VL; // the VL for the packet
}

//
// NOTE the generic message already have length in bits !!!
// 

message IBDataMsg extends IBWireMsg
{
  int appIdx;           // the application index within the SRC
  int msgIdx;           // a message ID unique to the SRC
  int msgLen;           // number of packets in the msg
  int pktIdx;           // packet index within the message
  int packetId;       	// globally unique packet ID 
  int packetSn;         // serial number of packet by the source to this dst 
  int flitSn;       	// serial number with the flit: 0..N 
  int PacketLength;     // Total number of credits in this packet. 
  int PacketLengthBytes;// Total packet Length in Bytes
  int dstLid;         	// target LID 
  int srcLid;         	// source LID 
  int SL;             	// the SL 
  int useStatic;       	// Used in ib_credits
  simtime_t SwTimeStamp;// Time stamp set when credit gets in IBRX    
  simtime_t injectionTime; // the time the credit hit the wire 
  bool beforeAnySwitch; // set to true by HCA and then to false by the first switch - to identify loops
  int isFECN;           //mark congested root
  int isBECN;           //carried in CNP
  int isAppMsg;
  double RecvRate;
}

message IBFlowControl extends IBWireMsg
{
  long FCTBS;
  long FCCL;
}

// this message used to signal a down port for upper level switch is loaded/unloaded
message IBTQLoadUpdateMsg extends IBWireMsg
{
  int load; // 0 - unloaded, 1 - loaded, 2 - overload
  int srcRank;
  int firstLid; // the LID range
  int lastLid;
}

///////////////////////////////////////////////////////////////////////////
//
// MESSAGES THAT ARE AVAILABLE INSIDE THE DEVICES
//


message IBSinkTimerMsg extends cMessage
{
  int srcLid;
}


// This message carry the "just" received flow control packet FCCL&FCTBS
message IBTxCredMsg extends cMessage
{
  int VL;
  long FCCL;
  long FCTBS;
}

// This message carry the FCCL from the the IBUF to the OBUF
message IBRxCredMsg extends cMessage
{
  int VL;
  long FCCL;
}

// FOR IB_CREDITS ONLY

// When a credit is sent by the arbiter it needs to notify the IBUF
// which packet was sent
message IBSentMsg extends cMessage
{
  short VL;
  short usedStatic;
  short wasLast;
}

// OBUF Informs the driver VLA about the number of credits it can hold
// this in turn can trigger a new arbitration.
// NOTE: to avoid races the arbiter only uses this info as a trigger for
// arbitration, it actually access the Q when it needs to know the Q free
// credits
message IBFreeMsg extends cMessage
{
  short VL;
}

// IBUF informs the VLAs it is driving about its ability to take a new
// arbitration
// NOTE: to avoid races the arbiter only uses this info as a trigger for
// arbitration, it actually access the IBUF BusyPorts structure to know if the
// arbitration succeeded message
message IBDoneMsg extends cMessage
{
  int appIdx;
  short VL;
}

// Applications sends this message that represent a transmission request to 
// one destination on some schedule queue
message IBAppMsg extends cMessage
{
  unsigned int appIdx;   // the application Idx in the SRC 
  unsigned int msgIdx;   // the application message index (uniq per APP)
  unsigned int SQ;       // the scheduler Q for this message
  unsigned int lenBytes; // the message length in bytes
  unsigned int lenPkts;  // the message length in packets
  unsigned int dstLid;   // destination lid
  unsigned int mtuBytes; // the MTU to be used in bytes
  // to be updated by the gen
  unsigned int pktLenFlits; // Current packet length in FLITs
  unsigned int pktLenBytes; // Current packet length in bytes
  unsigned int pktIdx;   // the next pkt index within the message
  unsigned int flitIdx;  // the next flit index in the packet
  unsigned int VL;       // the out VL to be used
}


message IBPushFECNMsg extends cMessage
{
  int srcLid;
  int dstLid;
  int SL;               // the SL
  int msgIdx;           // a message ID unique to the SRC
  int appIdx;           // the application index within the SRC
  double RecvRate;
  int BECNValue;           
}


message IBPushBECNMsg extends IBWireMsg
{
  int BECNValue;        //value of BECN. 1: congestion root, 2: congestion victim 3: non-congestion
  int srcLid;
  int dstLid;
  int SL;               // the SL
  int msgIdx;           // a message ID unique to the SRC
  int appIdx;           // the application index within the SRC
  double RecvRate;
  //int pktIdx;           // packet index within the message
}




