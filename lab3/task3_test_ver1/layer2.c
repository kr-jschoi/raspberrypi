#pragma once
#include "layer2.h"

void make_layer2() {
	send_frame = (frame*)malloc(sizeof(frame)); // Allocate memory
	receive_frame = (frame*)malloc(sizeof(frame)); // Allocate memory
}

void send_message() {
	int n;
	unsigned char m1[20] = "Send massage: ";
	uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
	for (n = 0; n < 20; n++) uart_transmit(m1[n]);
	for (n = 16; n < ((send_frame->size[0]-2) * 8)+16; n++) {
		if (read_bit(send_frame->payload, n)) { uart_transmit('1'); }
		else { uart_transmit('0'); }
	}
	send_complete = 0;
}

void received_message() {
	int n;
	int crc_correctness = crc_check(); // CRC Check

	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m1[20] = "CRC Check: ";
	for (n = 0; n < 20; n++) uart_transmit(m1[n]);
	if (crc_correctness == TRUE) { uart_transmit('O'); uart_transmit('K'); }
	else { uart_transmit('N'); uart_transmit('G'); }

	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m2[20] = "Received massage: ";
	for (n = 0; n < 20; n++) uart_transmit(m2[n]);
	for (n = 16; n < ((receive_frame->size[0]-2) * 8)+16; n++) {
		if (read_bit(receive_frame->payload, n)) { uart_transmit('1'); }
		else { uart_transmit('0'); }
	}
	receive_complete = 0;
}

int preamble_check(uint8_t *received_preamble) {
	if (send_preamble[0] == received_preamble[0]) return TRUE;
		return FALSE;
}

int crc_check() {
	int p;
	//uint8_t received_payload[PAYLOAD_SIZE];
	int crc_correctness = 0;
	/*
		for(p=0; p<(PAYLOAD_SIZE*8); p++){
			write_bit(received_payload, p, read_bit(send_frame->payload, p));
		}*/
	uint8_t* rechecked_crc = make_crc(receive_frame->size[0], receive_frame->payload); // Calculate CRC with received Payload

	for (p = 0; p < (CRC_SIZE * 8); p++) {
		write_bit(rechecked_crc, p, (read_bit(rechecked_crc, p) ^ read_bit(receive_frame->crc32, p))); // XOR rechecked crc with received CRC
		crc_correctness += read_bit(rechecked_crc, p); // Sum of result of XOR
	}
	return crc_correctness;
}

uint8_t* make_crc(uint8_t size, uint8_t * payload) {
	int l = 0, m = 0;
	static uint8_t crc32[CRC_SIZE] = { 0 };
	uint8_t poly[CRC_SIZE + 1] = { 0b10000010, // Polynomial
								   0b01100000,
								   0b10001110,
								   0b11011011,
								   0b10000000 };
	uint8_t *data = (uint8_t*)malloc((size+CRC_SIZE)*sizeof(uint8_t));

	for (l = 0; l < (size + CRC_SIZE); l++) {
		if (l < size) data[l] = payload[l];
		if (l >= size) data[l] = 0; // add 32 zeros at the end of the Payload
	}
	for (l = 0; l < (size * 8); l++) {
		if (read_bit(data, l) == 0) continue;
		for (m = 0; m < ((CRC_SIZE * 8) + 1); m++) {
			write_bit(data, (l + m), ((read_bit(data, (l + m))) ^ (read_bit(poly, m)))); // XOR with shift
		}
	}
	for (l = 0; l < (CRC_SIZE * 8); l++) {
		write_bit(crc32, l, read_bit(data, (size * 8) + l)); // remainder
	}
	return crc32;
}

void make_send_frame() {
	int k;
	send_frame->size[0] = 4;
	send_frame->payload[0] = 0b01110100;
	send_frame->payload[1] = 0b01100101;
	send_frame->payload[2] = 0b01110011;
	send_frame->payload[3] = 0b01110100;
	make_layer3(send_frame); // make Layer3

	uint8_t* sending_crc = make_crc(send_frame->size[0], send_frame->payload); // calculate CRC with sending Payload

	for (k = 0; k < CRC_SIZE; k++) {
		send_frame->crc32[k] = sending_crc[k];
	}
}

bool read_bit(uint8_t* bitString, int counter) {
	return(bitString[(counter / 8)] & (0b10000000 >> (counter % 8)));
}

void write_bit(uint8_t * bitString, int counter, uint8_t bitData) {
	if (bitData == 1) {
		bitString[(counter / 8)] |= ((0b10000000) >> (counter % 8));
	}
	else {
		bitString[(counter / 8)] &= (0b11111111 - ((0b10000000) >> (counter % 8)));
	}
}
