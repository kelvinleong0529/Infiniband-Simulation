# **Introduction**
- this projects aims to **simulate the data flow mechanism, explore performance + bottleneck** in [Infiniband](https://network.nvidia.com/related-docs/whitepapers/IB_Intro_WP_190.pdf) network specifically for distributed machine-learning systems in data center using [OMNeT++](https://omnetpp.org/) (based on [OMNeT++ Infiniband open source](https://omnetpp.org/download-items/InfiniBand.html))
- The details of the network simulation model are as below:
1. Networking Protocol: `Infiniband`
2. Data Transmission Principle: `Credit-based Flow Control`
3. Congestion Control Mechanism: `Congestion Control Table Indexing (BECN, FECN)`
4. Virtual Lane Selection Algorithm: `Weighted Round Robin (WRR)`
5. Data Transmission Framework: `Ring Allreduce`
6. Data Granularity: `Flow Control Unit (FLIT)`

# **Components**
## **1.Task Generator**
- create AI training / computational tasks; initialize the relevant data information & size in every `flit` , and forward it to [CPU](#2central-proccesing-unit-cpu) to assign the computing tasks among the nodes in the networking cluster
## **2.Central Controller**
- the core component that manages the entire networking cluster
- break down the AI training tasks received from [Task Generator](#1task-generator) ,and distributes it evenly among every nodes in the networking cluster
- generates the routing information for every `messages`, and updates the relevant routing tables in [switches](#6-switch)
## **3. Central Processing Unit (CPU)**
- responsible for generating computational tasks for [GPU](#4-graphic-processing-unit-gpu)
- when it receives AI training tasks from [Central Controller](#2central-controller), it forwards them to [GPU](#4-graphic-processing-unit-gpu) for computational training
## **4. Graphic Processing Unit (GPU)**
- responsible for AI training + relevant computation
- when it receives the data required for AI training from [CPU](#3-central-processing-unit-cpu), it performs the required computaion (model training), and forward it to [HCA](#5-host-channel-adapters-hca) and ready to be transmitted to the destination node
## **5. Host Channel Adapters (HCA)**
- a module that integrates [App](#7-application), [Gen](#8-generator), [Virtual Lane Arbiter](#9-virtual-lane-arbiter), [Output Buffer](#10-output-buffer), [Input Buffer](#11-input-buffer) & [Sink](#12-sink)
- similar to a `Network Interface Card (NIC)`, it encapsulates the trained data, and transmit them to the destination node through network link layer to update the AI model parameters
## **6. Switch**
- a module that integrates [Virtual Lane Arbiter](#8-virtual-lane-arbiter-vlarb), [Output Buffer](#10-output-buffer), [Input Buffer](#11-input-buffer) & [Packet Forwarder](#13-packet-forwarder)
- responsible for the information / `flit` exchange between different nodes in the networking cluster 
## **7. Application**
- component that connects the [HCA](#5-host-channel-adapters-hca) and lower-level [GPU](#4-graphic-processing-unit-gpu), responsible for coordinating the information / `flit` exchange between the 2 components
- Forwards the `flit` received from lower-level [GPU](#4-graphic-processing-unit-gpu) to [Generator](#8-generator)
- when [HCA](#5-host-channel-adapters-hca) receives a complete `message`, it notifies the lower-level [GPU](#4-graphic-processing-unit-gpu)
## **8. Generator**
- component in [HCA](#5-host-channel-adapters-hca) that is responsible for the implementation of `Infiniband` protocol
- breaks down the message received from [App](#7-application) into finer `packets, flits`, and analyze the main components in `flit` headers
- responsible for controlling the injection rate of the node, according to the selected congestion protocol, after receiving `Backward Explicit Congestion Notificaion (BECN)` value
## **9. Virtual Lane Arbiter**
- responsible for the arbitration on `Virtual Lane (VL)`, and select the appropriate Virtual Lane for data transmission based on real-time conditions
- temporarilt store the received data on a specific Virtual Lane, then check whether the node has enough credit to forward the `flit`, if criterias are fulfilled check which Virtual Lane is suitable for data transmission (based on `Weighted Round Robin` algorithm)
## **10. Output Buffer**
- a buffer that temporarily stored the `flit` before they are transmitted to another node
- updates the relevant credit values when the node successfuly processes some data
- when the destination node doesn't have enough buffers to receive the `flit`, put the `flit` inside a `queue`, else transmit the first `flit` inside the queue
## **11. Input Buffer**
- a buffer that temporarily stores `flit` after receiving them
- when it recieves `flit`, encapsulates it into a `message` by parsing the relevant content, and forward it to [Output Buffer](#10-output-buffer) and [Virtual Lane Arbiter](#9-virtual-lane-arbiter) to update the respective credits
- if this component is being applied inside the [Switch](#6-switch) module, it triggers the [Packet Forwarder](#13-packet-forwarder) component to determine the correct output port to transmit the data based on the destination node inside the `flit` header; if any congestion is detected, it triggers and set the `Forward Explicit Congestion Notification (FECN)` field to be true
## **12. Sink**
- component inside the server node that is responsible for collecting and processing the `flit` received
- responsible for checking whether the data arrives at the correct destination
- alert [Input Buffer](#11-input-buffer) to update the relevant credit value everytime it successfully process a `packet`
- notifies and update [Application](#7-application) after collecting a complete `message`
- if the `BECN` or `FECN` field of a received `flit` is **TRUE**, creates a `Congestion Notification Packet (CNP)` and forward it [Generator](#8-generator) to alleviate the congestion detected
## **13. Packet Forwarder**
- responsible for data or `flit` routing inside the [Switch](#6-switch) module
- establish a routing table based on the routing information received from [Central Controller](#2central-controller)
- determine the output port for data transmission based on the destination node field inside the `flit` received
