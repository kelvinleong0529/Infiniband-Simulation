//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 
import ib_model.*;
package ib_model.examples.evaluation_fattree128;

//128 fat-tree. 8-port 3-tree. 128 servers, 8*4 L1 switch; 8*4 L2 switch; 16 L3 switch
network fat_tree_128
{
    parameters:
        @display("bgb=443,428");
    submodules:
        servers[128]: HCA;
        L1_switches[32]: Switch {
            parameters:
                numSwitchPorts = 8;
                @display("p=182,58");
            gates:
                port[8];
        }
        L2_switches[32]: Switch {
            parameters:
                numSwitchPorts = 8;
                @display("p=175,129");
            gates:
                port[8];
        }
        L3_switches[16]: Switch {
            parameters:
                numSwitchPorts = 8;
                @display("p=120,224");
            gates:
                port[8];
        }
    connections allowunconnected:
        for i=0..127 {
            servers[i].port <--> IB4XQDRWire <--> L1_switches[int(i/4)].port[i%4];
        } 
        // aggr to core
        for j = 4..7 
        {
            L2_switches[0].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[0];
            L2_switches[1].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[0];
            L2_switches[2].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[0];
            L2_switches[3].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[0];
            
            L2_switches[4].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[1];
            L2_switches[5].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[1];
            L2_switches[6].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[1];
            L2_switches[7].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[1];
            
            L2_switches[8].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[2];
            L2_switches[9].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[2];
            L2_switches[10].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[2];
            L2_switches[11].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[2];
            
            L2_switches[12].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[3];
            L2_switches[13].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[3];
            L2_switches[14].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[3];
            L2_switches[15].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[3];
            //take care
            L2_switches[16].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[4];
            L2_switches[17].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[4];
            L2_switches[18].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[4];
            L2_switches[19].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[4];
            
            L2_switches[20].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[5];
            L2_switches[21].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[5];
            L2_switches[22].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[5];
            L2_switches[23].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[5];
            
            L2_switches[24].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[6];
            L2_switches[25].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[6];
            L2_switches[26].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[6];
            L2_switches[27].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[6];
            
            L2_switches[28].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4].port[7];
            L2_switches[29].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+1].port[7];
            L2_switches[30].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+2].port[7];
            L2_switches[31].port[j] <--> IB4XQDRWire <--> L3_switches[(j-4)*4+3].port[7];
            
        }  
        
        //edge to aggr     
        for j = 4..7 
        {
            L1_switches[0].port[j] <--> IB4XQDRWire <--> L2_switches[j-4].port[0];
            L1_switches[1].port[j] <--> IB4XQDRWire <--> L2_switches[j-4].port[1];
            L1_switches[2].port[j] <--> IB4XQDRWire <--> L2_switches[j-4].port[2];
            L1_switches[3].port[j] <--> IB4XQDRWire <--> L2_switches[j-4].port[3];
            
            L1_switches[4].port[j] <--> IB4XQDRWire <--> L2_switches[4+j-4].port[0];
            L1_switches[5].port[j] <--> IB4XQDRWire <--> L2_switches[4+j-4].port[1];
            L1_switches[6].port[j] <--> IB4XQDRWire <--> L2_switches[4+j-4].port[2];
            L1_switches[7].port[j] <--> IB4XQDRWire <--> L2_switches[4+j-4].port[3];
            
            L1_switches[8].port[j] <--> IB4XQDRWire <--> L2_switches[8+j-4].port[0];
            L1_switches[9].port[j] <--> IB4XQDRWire <--> L2_switches[8+j-4].port[1];
            L1_switches[10].port[j] <--> IB4XQDRWire <--> L2_switches[8+j-4].port[2];
            L1_switches[11].port[j] <--> IB4XQDRWire <--> L2_switches[8+j-4].port[3];
            
            L1_switches[12].port[j] <--> IB4XQDRWire <--> L2_switches[12+j-4].port[0];
            L1_switches[13].port[j] <--> IB4XQDRWire <--> L2_switches[12+j-4].port[1];
            L1_switches[14].port[j] <--> IB4XQDRWire <--> L2_switches[12+j-4].port[2];
            L1_switches[15].port[j] <--> IB4XQDRWire <--> L2_switches[12+j-4].port[3];
            
            L1_switches[16].port[j] <--> IB4XQDRWire <--> L2_switches[16+j-4].port[0];
            L1_switches[17].port[j] <--> IB4XQDRWire <--> L2_switches[16+j-4].port[1];
            L1_switches[18].port[j] <--> IB4XQDRWire <--> L2_switches[16+j-4].port[2];
            L1_switches[19].port[j] <--> IB4XQDRWire <--> L2_switches[16+j-4].port[3];
            
            L1_switches[20].port[j] <--> IB4XQDRWire <--> L2_switches[20+j-4].port[0];
            L1_switches[21].port[j] <--> IB4XQDRWire <--> L2_switches[20+j-4].port[1];
            L1_switches[22].port[j] <--> IB4XQDRWire <--> L2_switches[20+j-4].port[2];
            L1_switches[23].port[j] <--> IB4XQDRWire <--> L2_switches[20+j-4].port[3];
            
            L1_switches[24].port[j] <--> IB4XQDRWire <--> L2_switches[24+j-4].port[0];
            L1_switches[25].port[j] <--> IB4XQDRWire <--> L2_switches[24+j-4].port[1];
            L1_switches[26].port[j] <--> IB4XQDRWire <--> L2_switches[24+j-4].port[2];
            L1_switches[27].port[j] <--> IB4XQDRWire <--> L2_switches[24+j-4].port[3];
            
            L1_switches[28].port[j] <--> IB4XQDRWire <--> L2_switches[28+j-4].port[0];
            L1_switches[29].port[j] <--> IB4XQDRWire <--> L2_switches[28+j-4].port[1];
            L1_switches[30].port[j] <--> IB4XQDRWire <--> L2_switches[28+j-4].port[2];
            L1_switches[31].port[j] <--> IB4XQDRWire <--> L2_switches[28+j-4].port[3];
        }        
      
}

