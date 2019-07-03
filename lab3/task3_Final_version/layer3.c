#pragma once
#include "layer3.h"

//! This is the function to make a layer 3
/*!
   \param ID: Destination ID
   \param send_frame: the frame to send
   \return void
*/
void make_layer3(unsigned int ID, frame* send_frame) {
	int k;
	for (k = (send_frame->size[0]); k > 0; k--) {
		send_frame->payload[k + 1] = send_frame->payload[k - 1];
	}
	send_frame->payload[DESTINATION] = ID; // Destination ID
	send_frame->payload[SOURCE] = MY_ID; // Source ID
	send_frame->size[0] += 2; // Increase the size of payload by 2bytes
}

//! This is the function to check Source ID and Destination ID
/*!
   \param send_frame: the frame to send
   \param receive_frame: the frame to receive
   \return void
*/
void ID_check(frame *send_frame, frame *receive_frame) {
	uart_transmit('\r'); uart_transmit('\n');

	/* Source check */
	//! - When SOURCE_ID=MY_ID, that means the message I sent come back to my node
	if (source_check(receive_frame->payload[SOURCE]) == TRUE) { // Source check
		uart_transmit('\r'); uart_transmit('\n');
		print("[Returned MY_ID]", 16);
		receive_select = PREAMBLE_FLAG;
		send_select = USER_INPUT;
	}
	else{
		/* Destination check*/
		//! - When Destination ID is Zero(Broadcast), the message is received and relayed
		if ((receive_frame->payload[DESTINATION]) == BROADCAST) { // BROADCAST
			uart_transmit('\r'); uart_transmit('\n');
			print("[BOADCAST]", 10);

			receive_select = RECEIVE_COMPLETE;
			retransmit(send_frame, receive_frame);
		}
		//! - When Destination ID is same with My ID, the message is received
		else if ((receive_frame->payload[DESTINATION]) == MY_ID) { // ID is matched
			uart_transmit('\r'); uart_transmit('\n');
			print("[MY_ID]", 7);

			receive_select = RECEIVE_COMPLETE;
		}
		//! - When Destination ID is not my Id and Zero, the message is just relayed
		else {
			uart_transmit('\r'); uart_transmit('\n');
			print("[Not MY_ID]", 11);

			receive_select = PREAMBLE_FLAG; // enable to receive data mode
			retransmit(send_frame, receive_frame);
		}
	}
}

//! This is the function to check whether the source ID is matched with MY ID
/*!
   \param ID: Source ID to check with MY ID
   \return Same with MYID(True), not same with MYID(False)
*/
bool source_check(uint8_t ID) {
	if (ID == MY_ID) return TRUE;
	else return FALSE;
}

//! This is the function to retransmit the message I received
/*!
   \param send_frame: the frame to send
   \param receive_frame: the frame which is received
   \return void
*/
void retransmit(frame * send_frame, frame * receive_frame) {
	*send_frame = *receive_frame; // copy received frame to send frame

	relay_flag = 1; // relay flag ON(priority)
	send_msg_flag = 1; // 0: My send, 1: Retransmit
	
	send_select = PREAMBLE_FLAG; // enable to send data mode
}
