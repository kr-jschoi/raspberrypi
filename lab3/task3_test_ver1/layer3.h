#pragma once
#include "layer2.c"

#define BROADCAST 0
#define MY_ID 9
#define PRE_NODE_ID 15
#define NEXT_NODE_ID 15

#define PREAMBLE_FLAG 0
#define CRC_FLAG 1
#define SIZE_FLAG 2
#define PAYLOAD_FLAG 3
#define SEND_COMPLETE 4
#define RECEIVE_COMPLETE 5
#define LAYER3 6

volatile int send_select = PREAMBLE_FLAG, receive_select = PREAMBLE_FLAG;
//void make_layer3(frame *send_frame);
void ID_check(frame *send_frame, frame *receive_frame);
bool source_check(uint8_t ID);
void retransmit(frame * send_frame, frame * receive_frame);

/********************[Layer3 - Network Layer]**********************************
 1. packets are retransmitted through the ring if the packet needs
	to reach other recipient(s) or handing the packet to layer 4.

 2. The address zero (0x00) is used as broadcast-address. The
	addresses are assigned statically at start-up of the system.

 3. Whenever a layer 3 implementation receives a package, it checks
	the destination address. If the address is equal to the address
	gotten in initialization, the packet will be handed over to layer 4.

 4. Broadcast: the sender receives its own package, it knows that every
	addressee has received the message properly and notifies layer 4 of
	the successful sending operation.

 5. If the package is lost or corrupted on the way through the ring,
	layer 4 needs to care about resending or ignoring the fault.

 6. Whenever the layer 3 implementation receives a message with its own address
	or the broadcast-address set as source-address,it will discard the package.

 7. A CRC-check is not performed at the intermediate-nodes between
	the sender and the receiver.

 8. If the destination differs from the own address, the ATMega can begin relaying
	the read Bytes to the next node, while still receiving the remaining ones.

 9. An communication-error can be detected on the receivers end. This
	behaviour is also possible for broadcast-message, but every node has to
	check the destination address. If the receiving node sent the broadcast
	message relaying must not be done.

 10. Priorities: Packages that need to be relayed should be handled with
	 a higher priority than own sending-wishes of the node. Sending-wishes
	 should be handled in order of their arrival(FIFO).
********************************************************************************/
