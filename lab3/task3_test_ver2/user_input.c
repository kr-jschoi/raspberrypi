#pragma once
#include "layer3.c"

void user_input() {
	int k;
	unsigned char temp;
	unsigned char _ID[4], _SIZE[4];
	unsigned int ID=0, SIZE=0;

	/* ID Input */
	uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
	for (k = 0; k < 50; k++) uart_transmit('*');
	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m1[30] = "Enter ID to send(0~255): ";
	for (k = 0; k < 30; k++) uart_transmit(m1[k]);
	while (1) {
		for (k = 0; k < 3; k++) {
			_ID[k] = uart_receive();
			if (_ID[k] == ENTER) break; // Input "Enter" key
			uart_transmit(_ID[k]);
		}
		while (1) {
			if (_ID[k] == ENTER) break;
			_ID[k] = uart_receive();
			if (_ID[k] == ENTER) break;
		}
		break;
	}
	if(k==1) {ID = _ID[0]-48;} // ID with one digit
	else if(k==2) {ID = ((_ID[0]-48)*10 + (_ID[1]-48));} // ID with two digit
	else if(k==3) {ID = ((_ID[0]-48)*100 + (_ID[1]-48)*10 + (_ID[2]-48));} // ID with three digit

	/* Send message size*/
	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m2[40] = "Enter size(bytes) of message(1~251): ";
	for (k = 0; k < 40; k++) uart_transmit(m2[k]);
	while (1) {
		for (k = 0; k < 3; k++) {
			_SIZE[k] = uart_receive();
			if (_SIZE[k] == ENTER) break; // Input "Enter" key
			uart_transmit(_SIZE[k]);
		}
		while (1) {
			if (_SIZE[k] == ENTER) break;
			_SIZE[k] = uart_receive();
			if (_SIZE[k] == ENTER) break;
		}
		break;
	}
	if(k==1) {SIZE = _SIZE[0]-48;} // Size with one digit
	else if(k==2) {SIZE = ((_SIZE[0]-48)*10 + (_SIZE[1]-48));} // Size with two digit
	else if(k==3) {SIZE = ((_SIZE[0]-48)*100 + (_SIZE[1]-48)*10 + (_SIZE[2]-48));} // Size with three digit

	/* Send message input*/
	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m3[30] = "Enter send message(1 or 0): ";
	for (k = 0; k < 30; k++) uart_transmit(m3[k]);
	uint8_t* message = (uint8_t*)malloc(SIZE * sizeof(uint8_t)); // Allocate Memory

	while(1){
		for (k = 0; k < (SIZE * 8); k++) {
			if ((k % 8) == 0) uart_transmit(' ');
			if ((k % 32) == 0) { uart_transmit('\r'); uart_transmit('\n'); }

			temp = uart_receive();
			if (temp == ENTER) break;
			if (temp == '1') { uart_transmit('1'); write_bit(message, k, 1); }
			else if (temp == '0') { uart_transmit('0'); write_bit(message, k, 0); }
			else k--;
		}
		while(1){
			if (temp == ENTER) break;
			temp = uart_receive();
			if (temp == ENTER) break;
		}
		break;
	}

	make_send_frame(SIZE, ID, message);
	free(message); // remove memory
}
