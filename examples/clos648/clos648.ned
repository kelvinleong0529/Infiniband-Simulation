import ib_model.*;
package ib_model.examples.clos648;

network clos648
{
	submodules:
		H[648]: HCA {
			parameters:
				@display("i=device/server2;p=80,109");
		}

		sw_l1[36]: Switch {
			parameters:
				numSwitchPorts = 36;
				@display("p=182,58");
			gates:
				port[36];
		}

		sw_l2[18]: Switch {
			parameters:
				numSwitchPorts = 36;
				@display("p=182,58");
			gates:
				port[36];
		}

	connections:
		for i = 0..647
		{
			H[i].port <--> IB4XQDRWire <--> sw_l1[int(i/18)].port[i % 18];
		}
		for i = 0..17
		{
			sw_l1[0].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[0];
			sw_l1[1].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[1];
			sw_l1[2].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[2];
			sw_l1[3].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[3];
			sw_l1[4].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[4];
			sw_l1[5].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[5];
			sw_l1[6].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[6];
			sw_l1[7].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[7];
			sw_l1[8].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[8];
			sw_l1[9].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[9];
			sw_l1[10].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[10];
			sw_l1[11].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[11];
			sw_l1[12].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[12];
			sw_l1[13].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[13];
			sw_l1[14].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[14];
			sw_l1[15].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[15];
			sw_l1[16].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[16];
			sw_l1[17].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[17];
			sw_l1[18].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[18];
			sw_l1[19].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[19];
			sw_l1[20].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[20];
			sw_l1[21].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[21];
			sw_l1[22].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[22];
			sw_l1[23].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[23];
			sw_l1[24].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[24];
			sw_l1[25].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[25];
			sw_l1[26].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[26];
			sw_l1[27].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[27];
			sw_l1[28].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[28];
			sw_l1[29].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[29];
			sw_l1[30].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[30];
			sw_l1[31].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[31];
			sw_l1[32].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[32];
			sw_l1[33].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[33];
			sw_l1[34].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[34];
			sw_l1[35].port[i + 18] <--> IB4XQDRWire <--> sw_l2[i].port[35];
		}
}