#define F_CPU 12000000UL
#define BAUD 9600UL // BAUDRATE 9600
#define PD2_VOL (PIND & (1<<PIND2))
#define PREAMBLE_SIZE 1 // 1 Byte
#define CRC_SIZE 4 // 4 Byte
#define DLC 1 // 1 Byte
#define PAYLOAD_SIZE 4 // 4 Byte(Maximum: 251 Byte)
#define DATA_SIZE (PREAMBLE_SIZE+CRC_SIZE+DLC+PAYLOAD_SIZE) // Preamble(1Byte) + CRC(4Byte) + DLC(1Byte) + PAYLOAD_SIZE
#define TRUE 0
#define FALSE 1
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "init_setup.h"

void make_send_buffer();
uint8_t *make_crc(uint8_t *payload);
void send_message();
void received_message();
int preamble_check();
int crc_check();
bool read_bit(uint8_t* bitString, int counter);
void write_bit(uint8_t* bitString, int counter, uint8_t bitData);

volatile int i = 0, j = 0;
uint8_t send_buffer[DATA_SIZE] = { 0 };
uint8_t receive_buffer[DATA_SIZE] = { 0 };
volatile bool preamble_complete = 0; // set 1 after completing Preemble
volatile bool send_complete = 0; // set 1 after sending buffer
volatile bool receive_complete = 0; // set 1 after receiving buffer

/*[1]Physical layer: Clock signal(send:PB1->PD3)*/
ISR(TIMER1_COMPA_vect){
	PORTB ^= (1 << PB1); // Clock signal
}

/*[2]Data Link layer: send data(send:PB2->PD2)*/
ISR(TIMER1_COMPB_vect){	
	if(i < (PREAMBLE_SIZE*8)){ // send preemble
		if (read_bit(send_buffer, i) == 1) { PORTB |= (1 << PINB2); } // send data(PB2->PD2)
		else{  PORTB &= ~(1 << PINB2); }
		i++;
	}
	else if ((i < (DATA_SIZE*8))) {
		if (read_bit(send_buffer, i) == 1) { PORTB |= (1 << PINB2); } // send data(PB2->PD2)
		else { PORTB &= ~(1 << PINB2); }
		i++;
	}
	if(i == (DATA_SIZE * 8)) {send_complete = 1; i++; } // send complete
}

/*[1]Physical layer: Clock signal(recieve:PB1->PD3)*/
/*[2]Data Link layer: read data(receive:PD2<-PB2)*/
ISR(PCINT2_vect) {
	if(j < (PREAMBLE_SIZE * 8)){ // receive preamble
		if (PD2_VOL) {write_bit(receive_buffer, j, 1); } // read data(PD2<-PB2)
		else { write_bit(receive_buffer, j, 0); }
		j++;
	}
	else if ((j == 8) && (preamble_complete == 0)) {
		if (PD2_VOL) { receive_buffer[0] <<= 1; write_bit(receive_buffer, 7, 1); } // receive preamble until "01111110"
		else { receive_buffer[0] <<= 1; write_bit(receive_buffer, 7, 0); }
		if (preamble_check() == TRUE) preamble_complete = 1; // Preamble complete
	}
	else if ((j < (DATA_SIZE * 8)) && (preamble_complete == 1)){ // receive payload if preemble is "01111110"
		if (PD2_VOL) { write_bit(receive_buffer, j, 1); } // read data(PD2<-PB2)
		else { write_bit(receive_buffer, j, 0); }
		j++;
	}
	if(j == (DATA_SIZE * 8)) {receive_complete = 1; j++;} // recieve complete
}

/*******************************************MAIN**********************************************/
int main(void){
	io_port_setup(); // Output: PB1,PB2 / Input Port: PD2
	uart_init();
	timer_setup(); // period: 10ms
	pcint_setup(); // use PD3 PORT
	
	make_send_buffer();
	sei(); // grobal interrupt enable
	while (1) {
		if (send_complete == 1) {
			TIMSK1 &= ~(1 << OCIE1B); // Timer 1B Interrupt disable
			send_message(); 
		}	
		if (receive_complete == 1) {
			TIMSK1 &= ~(1 << OCIE1A); // Timer 1A Interrupt disable
			received_message();
		}
	}
}
/*********************************************************************************************/
void send_message(){ 
	int n;
	unsigned char m1[20] = "Send massage: ";
	uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
	for(n=0; n<20; n++) uart_transmit(m1[n]);
	for(n=0; n<(PAYLOAD_SIZE*8); n++) {
		if(read_bit(send_buffer, n+(PREAMBLE_SIZE + CRC_SIZE + DLC) * 8)) {uart_transmit('1');}
		else {uart_transmit('0');}
	}
	send_complete = 0;
}

void received_message(){
	int n;
	int crc_correctness = crc_check(); // CRC Check
	
	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m1[20] = "CRC Check: ";
	for(n=0; n<20; n++) uart_transmit(m1[n]);
	if(crc_correctness == TRUE) {uart_transmit('O');uart_transmit('K');}
	else {uart_transmit('N'); uart_transmit('G');}

	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m2[20] = "Received massage: ";
	for(n=0; n<20; n++) uart_transmit(m2[n]);
	for(n=0; n<(PAYLOAD_SIZE*8); n++) {
		if(read_bit(receive_buffer, n + (PREAMBLE_SIZE + CRC_SIZE + DLC) * 8)) {uart_transmit('1');}
		else {uart_transmit('0');}
	}
	receive_complete = 0;
}

int preamble_check() {
	uint8_t preamble[PREAMBLE_SIZE] = { 0b01111110 };
	if (preamble[0] == receive_buffer[0]) return TRUE;
	return FALSE;
}

int crc_check(){
	int p;
	uint8_t received_payload[PAYLOAD_SIZE];
	int crc_correctness=0;

	for(p=0; p<(PAYLOAD_SIZE*8); p++){
		write_bit(received_payload, p, read_bit(receive_buffer, p+((PREAMBLE_SIZE+CRC_SIZE+DLC)*8)));
	}
	uint8_t *rechecked_crc = make_crc(received_payload); // Calculate CRC with received Payload

	for(p=0; p<(CRC_SIZE*8); p++){
		write_bit(rechecked_crc, p, (read_bit(rechecked_crc, p) ^ read_bit(receive_buffer, p+((PREAMBLE_SIZE)*8)))); // XOR rechecked crc with received CRC
		crc_correctness += read_bit(rechecked_crc, p); // Sum of result of XOR
	}
	return crc_correctness;
}

uint8_t *make_crc(uint8_t *payload) {
	int l=0, m=0;
	static uint8_t crc32[CRC_SIZE] = {0};
	uint8_t poly[CRC_SIZE+1] = { 0b10000010, // Polynomial
							   	0b01100000,
								0b10001110,
								0b11011011,
								0b10000000 };
	
	uint8_t data[PAYLOAD_SIZE+CRC_SIZE] = {0};

	for(l=0; l<(PAYLOAD_SIZE+CRC_SIZE); l++){
		if(l<PAYLOAD_SIZE) data[l] = payload[l];
		if(l>=PAYLOAD_SIZE) data[l] = 0; // add 32 zeros at the end of the Payload
	}
	for(l=0; l<(PAYLOAD_SIZE*8); l++){
		if(read_bit(data, l) == 0) continue;
		for(m=0; m<((CRC_SIZE*8)+1); m++){	
			write_bit(data, (l+m), ((read_bit(data, (l+m))) ^ (read_bit(poly, m)))); // XOR with shift
		}
	}
	for(l=0; l<(CRC_SIZE*8); l++){
		write_bit(crc32, l, read_bit(data,(PAYLOAD_SIZE*8)+l)); // remainder
	}
	return crc32;
}

void make_send_buffer() {
	int k;
	uint8_t preamble[PREAMBLE_SIZE] = { 0b01111110 };
	uint8_t payload[PAYLOAD_SIZE]={ 0b01110100,
									0b01100101,
									0b01110011,
									0b01110100 };
	uint8_t *sending_crc = make_crc(payload); // calculate CRC with sending Payload
	
	for (k = 0; k < DATA_SIZE; k++) {
		if (k < PREAMBLE_SIZE) send_buffer[k] = preamble[k];
		if ((k >= PREAMBLE_SIZE) && (k < PREAMBLE_SIZE+CRC_SIZE)) send_buffer[k] = sending_crc[k - PREAMBLE_SIZE];
		if ((k >= PREAMBLE_SIZE + CRC_SIZE) && (k < PREAMBLE_SIZE + CRC_SIZE + DLC)) send_buffer[k] = PAYLOAD_SIZE;
		if (k >= PREAMBLE_SIZE + CRC_SIZE + DLC) send_buffer[k] = payload[k - (PREAMBLE_SIZE + CRC_SIZE + DLC)];
	}
}

bool read_bit(uint8_t *bitString, int counter) {
	return(bitString[(counter/8)] & (0b10000000 >> (counter%8)));
}

void write_bit(uint8_t* bitString, int counter, uint8_t bitData) {
	if(bitData == 1){ 
		bitString[(counter/8)] |= ((0b10000000) >> (counter%8));
	}else{
		bitString[(counter/8)] &= (0b11111111 - ((0b10000000) >> (counter%8)));
	}
}
