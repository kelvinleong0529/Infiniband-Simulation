package ib_model.examples.demo;

import ib_model.*;

network demo_network
{
    submodules:
        H_1: HCA {
            parameters:
                srcLid = 1;
                @display("i=device/server2;p=80,109");
        }
        H_2: HCA {
            parameters:
                srcLid = 2;
                @display("i=device/server2;p=80,191");
        }
        H_3: HCA {
            parameters:
                srcLid = 3;
                @display("i=device/server2;p=334,144");
        }

        H_4: HCA {
            parameters:
                srcLid = 4;
                @display("i=device/server2;p=256.80475,204.73375");
        }

        H_5: HCA {
            parameters:
                srcLid = 5;
                @display("i=device/server2;p=256.80475,85.79883");
        }

        H_6: HCA {
            parameters:
                srcLid = 6;
                @display("i=device/server2;p=216.56807,181.65683");
        }
        
        
        H_7: HCA {
            parameters:
                srcLid = 7;
                @display("i=device/server2;p=334,181.65683");
        }


        H_8: HCA {
            parameters:
                srcLid = 8;
                @display("i=device/server2;p=334,230.65683");
        }
        
        H_9: HCA {
            parameters:
                srcLid = 9;
                @display("i=device/server2;p=334,250.65683");
        }
        
        H_10: HCA {
            parameters:
                srcLid = 10;
                @display("i=device/server2;p=334,270.65683");
        }
        
        H_11: HCA {
            parameters:
                srcLid = 11;
                @display("i=device/server2;p=334,290.65683");
        }
        
        H_12: HCA {
            parameters:
                srcLid = 12;
                @display("i=device/server2;p=334,181.65683");
        }


        H_13: HCA {
            parameters:
                srcLid = 13;
                @display("i=device/server2;p=334,230.65683");
        }
        
        H_14: HCA {
            parameters:
                srcLid = 14;
                @display("i=device/server2;p=334,250.65683");
        }
        
        H_15: HCA {
            parameters:
                srcLid = 15;
                @display("i=device/server2;p=334,270.65683");
        }
        
        H_16: HCA {
            parameters:
                srcLid = 16;
                @display("i=device/server2;p=334,290.65683");
        }
        H_17: HCA {
            parameters:
                srcLid = 17;
                @display("i=device/server2;p=334,250.65683");
        }
        
        H_18: HCA {
            parameters:
                srcLid = 18;
                @display("i=device/server2;p=334,270.65683");
        }
        
        H_19: HCA {
            parameters:
                srcLid = 19;
                @display("i=device/server2;p=334,290.65683");
        }

        SW_1: Switch {
            parameters:
                numSwitchPorts = 18;
                @display("i=abstract/router;p=257,144");
            gates:
                port[18];
        }
        SW_2: Switch {
            parameters:
                numSwitchPorts = 3;
                @display("i=abstract/router;p=173,144");
            gates:
                port[3];
        }
    connections:
        H_1.port <--> IB4XQDRWire <--> SW_2.port[0];
        H_2.port <--> IB4XQDRWire <--> SW_2.port[1];
        SW_2.port[2] <--> IB4XQDRWire <--> SW_1.port[1];
        SW_1.port[0] <--> IB4XQDRWire <--> H_3.port;
        SW_1.port[2] <--> IB4XQDRWire <--> H_4.port;
        SW_1.port[3] <--> IB4XQDRWire <--> H_5.port;
        SW_1.port[4] <--> IB4XQDRWire <--> H_6.port;
        SW_1.port[5] <--> IB4XQDRWire <--> H_7.port;
        SW_1.port[6] <--> IB4XQDRWire <--> H_8.port;
        SW_1.port[7] <--> IB4XQDRWire <--> H_9.port;
        SW_1.port[8] <--> IB4XQDRWire <--> H_10.port;
        SW_1.port[9] <--> IB4XQDRWire <--> H_11.port;
        
        SW_1.port[10] <--> IB4XQDRWire <--> H_12.port;
        SW_1.port[11] <--> IB4XQDRWire <--> H_13.port;
        SW_1.port[12] <--> IB4XQDRWire <--> H_14.port;
        SW_1.port[13] <--> IB4XQDRWire <--> H_15.port;
        SW_1.port[14] <--> IB4XQDRWire <--> H_16.port;
        SW_1.port[15] <--> IB4XQDRWire <--> H_17.port;
        SW_1.port[16] <--> IB4XQDRWire <--> H_18.port;
        SW_1.port[17] <--> IB4XQDRWire <--> H_19.port;
}

