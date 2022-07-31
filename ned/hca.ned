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
package ib_model;

//
// This NED provides an HCA model

module HCA
{
    parameters:
        int srcLid;
        @display("bgb=340,225;i=device/server2");
    gates:
        inout port;

    submodules:
        app: IBRingAllreduceApp {
            parameters:
                @display("p=313,170");
        }
        gen: IBGenerator {
            parameters:
                srcLid = srcLid;
                @display("i=block/source;p=246,170");
            gates:
                in[1];
        }
        sink: IBSink {
            parameters:
                @display("i=block/sink;p=248,74");
        }
        obuf: IBOutBuf {
            parameters:
                isHcaOBuf = 1;
                @display("i=block/queue;p=69,169");
        }
        ibuf: IBInBuf {
            parameters:
                isHcaIBuf = 1;
                numPorts = 1;
                @display("i=block/subqueue;p=69,74");
            gates:
                out[1];
                sent[1];
        }
        vlarb: IBVLArb {
            parameters:
                isHcaArbiter = 1;
                numPorts = 1;
                @display("i=block/dispatch;p=158,170");
            gates:
                in[1];
                sent[1];

        }
    connections:
        app.out <--> gen.in[0];
        sink.out --> app.in;

        gen.out --> vlarb.in[0];
        vlarb.sent[0] --> { @display("m=m,68,20,68,20"); } --> gen.sent;
        vlarb.out --> { @display("m=m,16,56,24,60"); } --> obuf.in;
        obuf.free --> { @display("m=m,20,28,4,24"); } --> vlarb.free;

        ibuf.out[0] --> sink.in;
        sink.sent --> ibuf.sent[0];

        ibuf.rxCred --> { @display("m=m,28,12,36,12"); } --> obuf.rxCred;
        ibuf.txCred --> vlarb.txCred;

        port$i --> ibuf.in;
        obuf.out --> port$o;

        sink.pushFECN --> gen.recvFECN;
        sink.pushBECN --> gen.recvBECN;
}
