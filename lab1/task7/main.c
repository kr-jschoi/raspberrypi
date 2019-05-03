#define F_CPU 12000000UL
#define BAUD 19200UL // BAUDRATE
//#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1) //BAUDRATE = 103 : 0110 0111 / 16UL: Unsigned long
#define MYUBRR ((F_CPU/(16L*BAUD))-1)
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>

void uart_init(unsigned int ubrr); // function to initialize UART
void uart_transmit (unsigned char data); // function to send data
unsigned char uart_receive(void); // function to receive data
void ledModeSelect(unsigned char data); // function to select LED Mode
void showOutput(unsigned char data); // function to show Mode

int main(void)
{
	unsigned char data;
	uart_init(MYUBRR);

	while(1){
		data = uart_receive();
		ledModeSelect(data);
	}
}

// function to initialize UART
void uart_init(unsigned int ubrr){
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enanble receiver and transmitter */
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C |= (1<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
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

// function to select LED Mode
void ledModeSelect(unsigned char data){
	DDRB = 0xff;
	DDRD = 0xff;
	PORTB = 0x00;
	PORTD = 0x00;

	int i = 0;

		switch(data){
			case 'a':
				showOutput(data);
				for(i=0; i<6; i++){
					PORTD |= (0x04 << i);
					_delay_ms(300);
				}
				for(i=0; i<6; i++){
					PORTB |= (0x01 << i);
					_delay_ms(300);
				}
				break;
			case 's':
				showOutput(data);
				for(i=0; i<6; i++){
					PORTD |= (0x04 << i);
					_delay_ms(200);
				}
				for(i=0; i<6; i++){
					PORTB |= (0x01 << i);
					_delay_ms(200);
				}
				break;
			case 'd':
				showOutput(data);
				for(i=0; i<6; i++){
					PORTD |= (0x04 << i);
					_delay_ms(100);
				}
				for(i=0; i<6; i++){
					PORTB |= (0x01 << i);
					_delay_ms(100);
				}
				break;
		}
}

void showOutput(unsigned char data){
	int i = 0;
	unsigned char mode1[30] = "\r\nMode 1: Speed is 300ms";
	unsigned char mode2[30] = "\r\nMode 2: Speed is 200ms";
	unsigned char mode3[30] = "\r\nMode 3: Speed is 100ms";

	switch(data){
		case 'a':
			for(i=0; i<30; i++) uart_transmit(mode1[i]);
			break;
		case 's':
			for(i=0; i<30; i++) uart_transmit(mode2[i]);
			break;
		case 'd':
			for(i=0; i<30; i++) uart_transmit(mode3[i]);
			break;
	}
}
