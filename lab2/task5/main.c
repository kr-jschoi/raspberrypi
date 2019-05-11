#define F_CPU 12000000UL
#define BAUD 9600UL // BAUDRATE 9600
#define PD2_VOL (PIND & (1<<PIND2))
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

void io_port_setup(void);
void uart_init(void); // function to initialize UART
void uart_transmit (unsigned char data); // function to send data
unsigned char uart_receive(void); // function to receive data
void timer_setup(void); // timer setup
void pcint_setup(void); // pin change interrupt setup

volatile int i = 0;
volatile int receive_data[8];
volatile int receive_buffer = 0;
volatile int send_buffer = 0;
volatile int send_data[8] = { 1,1,0,0,1,0,1,0 };

/*[1]Physical layer: Clock signal(send:PB1->PD3)*/
ISR(TIMER1_COMPA_vect){
	PORTB ^= (1 << PB1); // Clock signal
}

/*[2]Data Link layer: send data(send:PB2->PD2)*/
ISR(TIMER1_COMPB_vect){	
	send_buffer = send_data[i];
	i++;
	if(i==8) i=0;
	PORTB ^= (1 << PB5); // LED PB5 Blinking
	if(send_buffer == 1) {PORTB |= (1 << PINB2);} // send data(PB2->PD2)
	else if(send_buffer == 0) {PORTB &= ~(1 << PINB2);}
}

/*[1]Physical layer: Clock signal(recieve:PB1->PD3)*/
/*[2]Data Link layer: read data(receive:PD2<-PB2)*/
ISR(PCINT2_vect) {
	if (PD2_VOL) {receive_buffer=1;uart_transmit('1');} // read data(PD2<-PB2)
	else {receive_buffer=0;uart_transmit('0');}
	
	PORTB ^= (1 << PB4); // LED PB4 Blinking
}

/*******************************************MAIN**********************************************/
int main(void)
{
	io_port_setup();
	uart_init();
	timer_setup();
	pcint_setup();
	
	while(1){
	}
}
/*********************************************************************************************/

void io_port_setup() {
	DDRB |= (1 << DDB1) | (1 << DDB2) | (1 << DDB4) | (1 << DDB5); // Output port(PB1,PB2)
	DDRD &= ~(1 << DDD2) & ~(1 << DDD3); // Input port(PD1,PD2)
	PORTD |= (1 << PD2); // Pull up registor
}

void uart_init(){
	UBRR0H = UBRRH_VALUE; // Set baud rate
	UBRR0L = UBRRL_VALUE;
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0); // Enanble receiver and transmitter
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00); // Set frame format: 8 data
}

void uart_transmit (unsigned char data){
	while(!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
	UDR0 = data; // Put data into buffer
}

unsigned char uart_receive(){
	while(!(UCSR0A & (1<<RXC0))); // Wait for data to be received
	return UDR0; // Get and return received 8 bit data from buffer
}

void timer_setup(){
	/* Timer0 CTC mode */
	TCCR1B |= (1<<WGM12)|(1<<CS12); // CTC mode / prescale=256
	OCR1AH = 0xB7; // 12MHz/256 = 46875 --> 46875Hz/46875timer = period:1s
	OCR1AL = 0x1B;
	OCR1BH = 0x5B; // 46875/2 = 23438 --> period:1s(0.5s shift)
	OCR1BL = 0x8E;
	TIMSK1 |= (1<<OCIE1A)|(1<<OCIE1B); // Timer/Counter0 Overflow Interrupt Enalbe(TOIE0)
	sei(); // grobal interrupt enable
}

void pcint_setup() {
	PCICR |= (1<<PCIE2); // Enable PCINT[23:16](PD3)
	PCMSK2 |= (1<<PCINT19); // Enable PCINT19(PD3) Mask
}