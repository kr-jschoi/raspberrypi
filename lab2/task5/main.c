#define F_CPU 12000000UL
#define BAUD 9600UL // BAUDRATE 9600
#define MYUBRR ((F_CPU/(16L*BAUD))-1)
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

void uart_init(unsigned int ubrr); // function to initialize UART
void uart_transmit (unsigned char data); // function to send data
unsigned char uart_receive(void); // function to receive data
void timer_setup(void); // timer setup
void pcint_setup(void); // pin change interrupt setup

int count0=0;
int count1=0;
ISR(TIMER0_OVF_vect){
	TCNT0 = 209 ; // set TCNT0 initial value for 1ms
				  // 12MHz --> 12000000Hz / 256(prescale) = 46875Hz
				  // period(1/46875Hz) = 21us
				  // 1.0026ms = 47*21.3333us
				  // TCNT initial value = 256(8bits) - 47 = 209
	count0++;
	if(count0>1000){ // 0.10026s = 100*1.0026ms
//		PORTB ^= (1 << PINB1); // [1]Physical layer: Sending clock signal by PB1 blinking
//		PORTB ^= (1 << PINB2); // Data send		
		PORTB ^= (1 << PINB4);
		count0 = 0;
	}
}

ISR(TIMER1_COMPA_vect){ 
	TCNT1H=0x00;
	TCNT1L=0xd1;
	OCR1AH=0x01;
	OCR1AL=0x00;
	count1++;
	if(count1>1000){
		PORTB ^= (1 << PINB5);
		count1 = 0;
	}
}
/*
ISR(PCINT2_vect) { // [1]Physical layer: receive clock(PD1 is synchronized with PB1) 
	PORTB ^= (1 << PINB4); // LED PB4, PB5 ON and OFF
}*/
/*
ISR(TIMER0_COMPB_vect){
	TCNT2 = 80;
	OCR2A = 127;
	
	countB++;
	if(countB>500){
		PORTB ^= (1 << PINB5);
		countB = 0;
	}
}
*/
int main(void)
{
	DDRB = 0xff; // Output port
	DDRD = 0x00; // Input port

	uart_init(MYUBRR);
	timer_setup();
	pcint_setup();

	while(1){
		uart_transmit('M');
		_delay_ms(100);
	}
}

// function to initialize UART
void uart_init(unsigned int ubrr){
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enanble receiver and transmitter */
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data*/
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
}

// function to send data
void uart_transmit (unsigned char data){
	/* Wait for empty transmit buffer */
	while(!(UCSR0A & (1<<UDRE0)));
	/* Put data into buffer */
	UDR0 = data;
}

// function to receive data
unsigned char uart_receive(void){
	/* Wait for data to be received */
	while(!(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0; // return 8-bit data
}

void timer_setup(){
	/* Timer0 OVERFLOW mode */
	TCCR0B |= (1<<CS02); // prescaler: 256(CS02=1)
	TCNT0 = 209;
	TIMSK0 |= (1<<TOIE0); // Timer/Counter0 Overflow Interrupt Enalbe(TOIE0)
	
	/* Timer1 OVERFLOW mode */
	TCCR1A |= (1<<WGM12); // CTC Mode
	TCCR1B |= (1<<CS12); // prescaler: 256(CS12=1)
	TCNT1H = 0x00;
	TCNT1L = 0xd1;
	OCR1AH = 0x01;
	OCR1AL = 0x00;
	TIMSK1 |= (1<<OCIE1A);

	/*
	// Timer 2 CTC mode
	TCCR2A |= (1<<WGM21); // CTC mode
	TCCR2B |= (1<<CS22)|(1<<CS21); // prescaler: 256
	TCNT2 = 80;
	OCR2A = 127; // 80~127(47)
	TIMSK2 |= (1<<OCIE2A);*/
	sei(); // grobal interrupt enable
}

void pcint_setup(void) {
	PCICR |= (1<<PCIE2); // Enable PCINT17,18(PD1, PD2)
	PCMSK2 |= (1<<PCINT17)|(1<<PCINT18); // Enable PCINT17,18(PD1, PD2) Mask
}
