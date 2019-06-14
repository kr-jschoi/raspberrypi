#pragma once
#include "layer3.h"

void make_layer3(unsigned int ID, frame* send_frame) {
	int k;
	for (k = (send_frame->size[0]); k > 0; k--) {
		send_frame->payload[k + 1] = send_frame->payload[k - 1];
	}
	send_frame->payload[DESTINATION] = ID;// destination ID
	send_frame->payload[SOURCE] = MY_ID;// Source ID
	send_frame->size[0] += 2;
}

void ID_check(frame *send_frame, frame *receive_frame) {
	int k;

	uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
	unsigned char m0[20] = "Source ID: ";
	for (k = 0; k < 20; k++) uart_transmit(m0[k]);
	for(k=8; k<16; k++){
		if(read_bit(receive_frame->payload, k)) uart_transmit('1');
		else uart_transmit('0');
	}
	
	/*If SOURCE_ID=MY_ID, that means there is no matching node in the network.*/
	if (source_check(receive_frame->payload[SOURCE]) == TRUE) { // Source check
		uart_transmit('\r'); uart_transmit('\n');
		unsigned char m1[20] = "[Returned MY_ID]";
		for (k = 0; k < 20; k++) uart_transmit(m1[k]);
		uart_transmit('\r'); uart_transmit('\n');
		receive_select = PREAMBLE_FLAG;
		send_select = USER_INPUT;
	}
	else{
		/* Destination check*/
		if ((receive_frame->payload[DESTINATION]) == BROADCAST) { // BROADCAST
			uart_transmit('\r'); uart_transmit('\n');
			unsigned char m1[20] = "[BROADCAST]";
			for (k = 0; k < 20; k++) uart_transmit(m1[k]);
			uart_transmit('\r'); uart_transmit('\n');

			receive_select = RECEIVE_COMPLETE;
			retransmit(send_frame, receive_frame);
		}
		else if ((receive_frame->payload[DESTINATION]) == MY_ID) {
			uart_transmit('\r'); uart_transmit('\n');
			unsigned char m2[10] = "[MY_ID]";
			for (k = 0; k < 10; k++) uart_transmit(m2[k]);
			uart_transmit('\r'); uart_transmit('\n');

			receive_select = RECEIVE_COMPLETE;
		}
		else {// ID is not matched
			uart_transmit('\r'); uart_transmit('\n');
			unsigned char m3[20] = "[Not MY_ID]";
			for (k = 0; k < 20; k++) uart_transmit(m3[k]);
			uart_transmit('\r'); uart_transmit('\n');

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
	int k;
	*send_frame = *receive_frame;

	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m1[20] = "[Retransmit]";
	for(k=0; k<20; k++) uart_transmit(m1[k]);
//	for (k = 16; k < ((send_frame->size[0] - 2) * 8) + 16; k++) {
//		if(k != 16){
//			if((k % 8) == 0) uart_transmit(' ');
//			if(((k - 16) % 32) == 0) {uart_transmit('\r'); uart_transmit('\n');}
//		}
//		if (read_bit(send_frame->payload, k)) { uart_transmit('1'); }
//		else { uart_transmit('0'); }
//	}
	
	send_select = PREAMBLE_FLAG; // enable to send data mode
}
