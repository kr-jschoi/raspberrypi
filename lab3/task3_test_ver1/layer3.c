#pragma once
#include "layer3.h"

void make_layer3(frame* send_frame) {
	int k;
	for (k = (send_frame->size[0]); k > 0; k--) {
		send_frame->payload[k + 1] = send_frame->payload[k - 1];
	}
	send_frame->payload[0] = NEXT_NODE_ID;// destination ID
	send_frame->payload[1] = MY_ID;// Source ID
	send_frame->size[0] += 2;
}

void ID_check(frame *send_frame, frame *receive_frame) {
	int k;
	if ((receive_frame->payload[0]) == BROADCAST){ // BROADCAST
		uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
		unsigned char m1[20] = "ID CHECK: BROADCAST";
		for(k=0; k<20; k++) uart_transmit(m1[k]);

		retransmit(send_frame, receive_frame);
		receive_select = RECEIVE_COMPLETE;
	}
	else if((receive_frame->payload[0]) == MY_ID){
		uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
		unsigned char m2[30] = "ID CHECK: ID is matched";
		for(k=0; k<30; k++) uart_transmit(m2[k]);
		
		receive_select = RECEIVE_COMPLETE;
	}
	else{// ID not matched
		uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
		unsigned char m3[30] = "ID CHECK: ID is not matched";
		for(k=0; k<30; k++) uart_transmit(m3[k]);
	
		retransmit(send_frame, receive_frame);
		receive_select = 100;
	}
}

bool source_check(uint8_t ID) {
	if (ID == MY_ID) return TRUE;
	else return FALSE;
}

void retransmit(frame * send_frame, frame * receive_frame) {
	int k;
	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m1[20] = "Retransmit message";
	for(k=0; k<20; k++) uart_transmit(m1[k]);
	
	*send_frame = *receive_frame;
}
