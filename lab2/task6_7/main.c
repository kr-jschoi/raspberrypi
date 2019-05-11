#define F_CPU 12000000UL
#define BAUD 9600UL // BAUDRATE 9600
#define PD2_VOL (PIND & (1<<PIND2))
#define PAYLOAD_SIZE 32 // MAXIMUM:208 because buffer size is limited
#define DATA_SIZE (8+32+8+PAYLOAD_SIZE)
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include "init_setup.h"

void make_send_buffer();
bool *make_crc(bool *payload);
void send_message();
void received_message();
int preemble_check();
int crc_check();

volatile int i = 0, j = 0;
volatile bool send_buffer[256]={1};
volatile bool receive_buffer[256]={1};
volatile bool preemble_receive = 0; // set 1 after receiving Preemble
volatile bool preemble_complete = 0; // set 1 after completing Preemble
volatile bool preemble_flag = 1; // 0: correct, 1: fail
volatile bool send_complete = 0; // set 1 after sending buffer
volatile bool receive_complete = 0; // set 1 after receiving buffer

/*[1]Physical layer: Clock signal(send:PB1->PD3)*/
ISR(TIMER1_COMPA_vect){
	PORTB ^= (1 << PB1); // Clock signal
}

/*[2]Data Link layer: send data(send:PB2->PD2)*/
ISR(TIMER1_COMPB_vect){	
	if(i<8){ // send preemble
		if (send_buffer[i] == 1) { PORTB |= (1 << PINB2); } // send data(PB2->PD2)
		else if (send_buffer[i] == 0) { PORTB &= ~(1 << PINB2); }
		i++;
	}else if ((i < DATA_SIZE) && (preemble_flag == 0)) { // send payload if preemble is "01111110"
		if (send_buffer[i] == 1) { PORTB |= (1 << PINB2); } // send data(PB2->PD2)
		else if (send_buffer[i] == 0) { PORTB &= ~(1 << PINB2); }
		i++;
	}
	if(i == DATA_SIZE) {send_complete = 1; i++;} // send complete
}

/*[1]Physical layer: Clock signal(recieve:PB1->PD3)*/
/*[2]Data Link layer: read data(receive:PD2<-PB2)*/
ISR(PCINT2_vect) {
	if(j<8){ // receive preemble
		if (PD2_VOL) {receive_buffer[j] = 1;} // read data(PD2<-PB2)
		else {receive_buffer[j] = 0;}
		j++;
		if(j==8) preemble_receive = 1;
	}if ((j < DATA_SIZE) && (preemble_flag == 0)){ // receive payload if preemble is "01111110"
		if (PD2_VOL) {receive_buffer[j] = 1;} // read data(PD2<-PB2)
		else {receive_buffer[j] = 0;}
		j++;
	}
	if(j==DATA_SIZE) {receive_complete = 1; j++;} // recieve complete
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
		if((preemble_receive ==1) && (preemble_complete == 0)){
			preemble_flag = preemble_check(); // Preemble check
		}
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
	uart_transmit('\r'); uart_transmit('\n');
	for(n=0; n<20; n++) uart_transmit(m1[n]);
	for(n=0; n<PAYLOAD_SIZE; n++) {
		if(send_buffer[n+48]) {uart_transmit('1');}
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
	if(crc_correctness == 0) {uart_transmit('O');uart_transmit('K');}
	else {uart_transmit('N'); uart_transmit('G');}

	uart_transmit('\r'); uart_transmit('\n');
	unsigned char m2[20] = "Received massage: ";
	for(n=0; n<20; n++) uart_transmit(m2[n]);
	for(n=0; n<PAYLOAD_SIZE; n++) {
		if(receive_buffer[n+48]) {uart_transmit('1');}
		else {uart_transmit('0');}
	}
	receive_complete = 0;
}

int preemble_check() {
	int q;
	int preemble_correctness=0;
	bool preemble[8] = { 0,1,1,1, 1,1,1,0 };

	unsigned char m1[20] = "Preemble Check: ";
	uart_transmit('\r'); uart_transmit('\n'); uart_transmit('\n');
	for (q = 0; q < 20; q++) uart_transmit(m1[q]);
	
	for (q = 0; q < 8; q++) {
		preemble[q] ^= receive_buffer[q]; // XOR received preemble with "01111110"
		preemble_correctness += preemble[q]; // Sum of result of XOR
	}

	if (preemble_correctness == 0) { uart_transmit('O'); uart_transmit('K'); }
	else { uart_transmit('N'); uart_transmit('G'); }
	
	preemble_complete = 1; // Preemble complete
	return preemble_correctness;
}

int crc_check(){
	int p;
	bool received_payload[PAYLOAD_SIZE];
	
	int crc_correctness=0;
	for(p=0; p<PAYLOAD_SIZE; p++){
		received_payload[p] = receive_buffer[p+48];
	}
	bool *rechecked_crc = make_crc(received_payload); // Calculate CRC with received Payload
	
	for(p=0; p<32; p++){
		rechecked_crc[p] ^= receive_buffer[p+8]; // XOR rechecked crc with received CRC
		crc_correctness += rechecked_crc[p]; // Sum of result of XOR
	}
	return crc_correctness;
}

bool *make_crc(bool *payload) {
	int l=0, m=0;
	static bool crc32[32] = {1};
	bool poly[33] = {       1, 0,0,0,0, // Polynomial
					  0,1,0,0, 1,1,0,0,
					  0,0,0,1, 0,0,0,1,
					  1,1,0,1, 1,0,1,1,
					  0,1,1,1};
	bool data[PAYLOAD_SIZE+32] = {1};
	
	for(l=0; l<(PAYLOAD_SIZE+32); l++){
		if(l<PAYLOAD_SIZE) data[l] = payload[l];
		if(l>=PAYLOAD_SIZE) data[l] = 0; // add 32 zeros at the end of the Payload
	}
	for(l=0; l<PAYLOAD_SIZE; l++){
		if(data[l] == 0) continue;
		for(m=0; m<33; m++){
			data[l+m] ^= poly[m]; // XOR with shift
		}
	}
	for(l=0; l<32; l++){
		crc32[l] = data[PAYLOAD_SIZE+l]; // remainder	
	}
	return crc32;
}

void make_send_buffer() {
	int k;
	bool preemble[8] = { 0,1,1,1, 1,1,1,0 };
	bool size[8] = { 0,0,0,0, 0,1,0,0 }; // Payload size: 4 Byte
	bool payload[PAYLOAD_SIZE]={ 1,1,0,0, 1,0,0,1, // 4 Byte
								 0,0,1,1, 0,0,1,0,
								 0,1,1,1, 0,0,0,1,
								 1,1,1,1, 0,1,0,0};
	bool *sending_crc = make_crc(payload); // calculate CRC with sending Payload
	for (k = 0; k < DATA_SIZE; k++) {
		if (k < 8) send_buffer[k] = preemble[k];
		if ((k >= 8) && (k < 40)) send_buffer[k] = sending_crc[k - 8];
		if ((k >= 40) && (k < 48)) send_buffer[k] = size[k - 40];
		if (k >= 48) send_buffer[k] = payload[k - 48];
	}
}
