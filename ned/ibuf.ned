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
// IBUF: Input Buffer
//
simple IBInBuf
{
 parameters:
  // input line width 4x,8x,12x
  int width; 
  // if 1 means the input buffer is part  of HCA (no routing required)
  int isHcaIBuf;
  // number of output ports to drive to
  int numPorts;
  // max number of parallel sends = switch "speed-up"
  int maxBeingSent;
  // the total buffer size in credits
  int totalBufferSize; 
  // max VL num. If only VL0 value is 0
  int maxVL;
  // declares the buffer as lossy - drop packets if full but lie about the free credits
  bool lossyMode;

  // Each VL is assigned a max number of static credits 
  int maxStatic0;
  int maxStatic1;
  int maxStatic2;
  int maxStatic3;
  int maxStatic4;
  int maxStatic5;
  int maxStatic6;
  int maxStatic7;
 gates:
  input in;
  input sent[];
  output out[];
  output txCred;
  output rxCred;
}


