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
// OBUF: send messages on the wire (merging flow control packets)
//
simple IBOutBuf
{
    parameters:
		bool enabled = default(true); // set to false to turn OFF the link
        int size;                  // the number of flits the Q can store
        int maxVL;                    // Maximum VL we send on
        double credMinTime @unit(us); // time between VL Credit packets
						  // NOTE: need to adjust if width change
        int isHcaOBuf;
        int on_throughput_obuf = default(0);
        double timeStep_us @unit(us); // timestep for throughput calculation
    gates:
        input in;
        input rxCred;
        output out;
        output free;
}


