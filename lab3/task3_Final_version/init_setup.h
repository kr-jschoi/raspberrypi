#pragma once
#define F_CPU 12000000UL
#define BAUD 9600UL // BAUDRATE 9600

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include <stdbool.h>

//! This is the function to set up IO ports
/*!
   \param void
   \return void
*/
void io_port_setup() {
	DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB4) | (1 << DDB5); // Output port(PB1,PB2)
	DDRD &= ~(1 << DDD2) & ~(1 << DDD3); // Input port(PD1,PD2)
	PORTD |= (1 << PD2); // Pull up registor
}

//! This is the function to set up the UART transmission
/*!
   \param void
   \return void
*/
void uart_init(){
	UBRR0H = UBRRH_VALUE; // Set baud rate as 9600
	UBRR0L = UBRRL_VALUE;
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0); // Enanble receiver and transmitter
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); // Set frame format: 8 data
}

//! This is the function to transmit a data by UART
/*!
   \param data: send 8bit character to buffer
   \return void
*/
void uart_transmit (unsigned char data){
	while(!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer
}

//! This is the function to receive a data by UART
/*!
   \param void
   \return uart_receive: return received 8 bit data from buffer
*/
unsigned char uart_receive() {
	while (!(UCSR0A & (1 << RXC0))); // Wait for data to be received
	return UDR0; // Get and return received 8 bit data from buffer
}

//! This is the function to set up the timer
/*!
   \param void
   \return void
*/
void timer_setup(){
	//! Timer is set up with 1ms period with CTC mode
	TCCR1B |= (1<<WGM12)|(1<<CS12); // CTC mode / prescale=256
	OCR1AH = 0x01; // 12MHz/256 = 46875 --> 46875Hz/ OCR:469 = period:0.01s
	OCR1AL = 0xD5;
	OCR1BH = 0x00; // 469/2 = 234 --> period:0.01s(0.005s shift)
	OCR1BL = 0xEA;
	TIMSK1 |= (1<<OCIE1A)|(1<<OCIE1B); // Timer/Counter1-A,B enable
}

//! This is the function to set up Pin Change Interrupt
/*!
   \param void
   \return void
*/
void pcint_setup() {
	PCICR |= (1<<PCIE2); // Enable PCINT[23:16](PD3)
	PCMSK2 |= (1<<PCINT19); // Enable PCINT19(PD3) Mask
}
