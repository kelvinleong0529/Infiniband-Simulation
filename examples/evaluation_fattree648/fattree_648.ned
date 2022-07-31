package ib_model.examples.evaluation_fattree648;

import ib_model.*;

network fattree_648_network
{
    submodules:        
        servers[648]: HCA {}
        leaf_switches[36]: Switch {
            parameters:
                numSwitchPorts = 36;
            gates:
                port[36];
        }
        spine_switches[18]: Switch {
            parameters:
                numSwitchPorts = 36;
            gates:
                port[36];      
        }
    connections:
        for i=0..647 {
            servers[i].port <--> IB4XQDRWire <--> leaf_switches[i/18].port[i%18];
        }
        for i=0..35, for j=18..35 {
            leaf_switches[i].port[j] <--> IB4XQDRWire <--> spine_switches[j-18].port[i];
        }

}

