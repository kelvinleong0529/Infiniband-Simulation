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
// VLArb: Packet Arbiter
//
simple IBVLArb
{
    parameters:
        int isHcaArbiter;        // if not zero means the VLA is in HCA
        int numPorts;   	      // number of input ports 
        int coreFreq @unit(Hz);  // Core frequency in MHz
        int busWidth @unit(B);   // Switch bus width in bytes (from in-buf 2 vla)
        int maxVL;               // Maximum VL we send on
        int vlHighLimit;         // IB VL Arb High Limit
        string highVLArbEntries; // "vl:weight vl:weight ..."
        string lowVLArbEntries;  // "vl:weight vl:weight ..."
        bool useFCFSRQArb;       // if true will arbitrate RQs on same VL by oldest
        int markrate;

    gates:
        input in[];
        input txCred;
        input free;
        output out;
        output sent[];
}

