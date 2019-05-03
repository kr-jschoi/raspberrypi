#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

void ledOn(void);
void ledOff(void);

int main()
{
	DDRB = 0xff; // activate PORTB for LED
	DDRD = 0xff; // activate PORTD for LED

    while(1)
    {
		ledOn();
		ledOff();
    }
}

void ledOn(void){
	int i=0;

	for(i=0; i<6; i++){
		PORTD |= (0x04 << i); // turn on LEDs of PORTD
		_delay_ms(200);
	}
		
	for(i=0; i<6; i++){
		PORTB |= (0x01 << i); // turn on LEDs of PORTB
		_delay_ms(200);
	}
}

void ledOff(void){
	int i=0;

	for(i=0; i<6; i++){
		PORTB &= ~(0x20 >> i); // turn off LEDs of PORTB
		_delay_ms(200);
	}

	for(i=0; i<6; i++){
		PORTD &= ~(0x80 >> i); // turn off LEDs of PORTD
		_delay_ms(200);
	}
}
