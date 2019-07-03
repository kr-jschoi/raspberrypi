#pragma once
#include "layer3.h"

void make_layer3(unsigned int ID, frame* send_frame) {
	int k;
	for (k = (send_frame->size[0]); k > 0; k--) {
		send_frame->payload[k + 1] = send_frame->payload[k - 1];
	}
	send_frame->payload[DESTINATION] = ID; // Destination ID
	send_frame->payload[SOURCE] = MY_ID; // Source ID
	send_frame->size[0] += 2; // Increase the size of payload by 2bytes
}

void ID_check(frame *send_frame, frame *receive_frame) {
	uart_transmit('\r'); uart_transmit('\n');

	/*If SOURCE_ID=MY_ID, that means there is no matching node in the network.*/
	/* Source check */
	if (source_check(receive_frame->payload[SOURCE]) == TRUE) { // Source check
		uart_transmit('\r'); uart_transmit('\n');
		print("[Returned MY_ID]", 16);
		receive_select = PREAMBLE_FLAG;
		send_select = USER_INPUT;
	}
	else{
		/* Destination check*/
		if ((receive_frame->payload[DESTINATION]) == BROADCAST) { // BROADCAST
			uart_transmit('\r'); uart_transmit('\n');
			print("[BOADCAST]", 10);

			receive_select = RECEIVE_COMPLETE;
			retransmit(send_frame, receive_frame);
		}
		else if ((receive_frame->payload[DESTINATION]) == MY_ID) { // ID is matched
			uart_transmit('\r'); uart_transmit('\n');
			print("[MY_ID]", 7);

			receive_select = RECEIVE_COMPLETE;
		}
		else {// ID is not matched
			uart_transmit('\r'); uart_transmit('\n');
			print("[Not MY_ID]", 11);

			receive_select = PREAMBLE_FLAG; // enable to receive data mode
			retransmit(send_frame, receive_frame);
		}
	}
}

bool source_check(uint8_t ID) {
	if (ID == MY_ID) return TRUE;
	else return FALSE;
}

void retransmit(frame * send_frame, frame * receive_frame) {
	*send_frame = *receive_frame;

	relay_flag = 1; // relay flag ON(priority)
	send_msg_flag = 1; // 0: My send, 1: Retransmit
	
	send_select = PREAMBLE_FLAG; // enable to send data mode
}
