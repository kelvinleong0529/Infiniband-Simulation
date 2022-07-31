import ib_model.*;
package ib_model.examples.tree32_1;

network tree32_1
{
    submodules:
        H[32]: HCA {
            parameters:
                @display("i=device/server2;p=80,109");
        }

        sw[4]: Switch {
            parameters:
                numSwitchPorts = 9;
                @display("p=182,58");
            gates:
                port[9];
        }

        core: Switch {
            parameters:
                numSwitchPorts = 4;
                @display("p=182,58");
            gates:
                port[4];
        }
    
    connections:
        for i = 0..31
        {
            H[i].port <--> IB4XQDRWire <--> sw[int(i/8)].port[i % 8];
        }
        for i = 0..3
        {
            sw[i].port[8] <--> IB4XQDRWire <--> core.port[i];
        }
}