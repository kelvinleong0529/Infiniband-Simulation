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
// Destroys the packets and collects statistics
//
simple IBSink
{
 parameters:
  double flitSize @unit(B);       // flit size in bytes
  double popDlyPerByte @unit(ns);   // the PCI Exp drain rate
  int maxVL;                      // Maximum VL we send on
  volatile double hiccupDuration @unit(us); // the time a hiccup may take 
  volatile double hiccupDelay @unit(us);    // the time between hiccups
  double startStatCol @unit(s);  // Time to start statistics collection from
  int repFirstPackets; // number of first packets that if are accepted record message latency done 
  int on_cc = default(0);            //IBA congestion control
  int on_newcc = default(0);         //new congestion control
  double timeStep_us @unit(us); // timestep for throughput calculation
  int on_utilization = default(0);
 gates:
  input in;
  output sent;
  output out;
  output pushFECN;
  output pushBECN;
}
