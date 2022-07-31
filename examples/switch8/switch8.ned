import ib_model.*;
package ib_model.examples.switch8;

network switch8
{
    submodules:
        H[8]: HCA {
            parameters:
                @display("i=device/server2;p=80,109");
        }

        sw: Switch {
            parameters:
                numSwitchPorts = 8;
                @display("p=182,58");
            gates:
                port[8];
        }
    
    connections:
        for k = 0..7 
        {
            H[k].port <--> IB4XQDRWire <--> sw.port[k];
        }
}