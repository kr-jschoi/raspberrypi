#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

int main()
{
	DDRB = 0xff;    // Make pin 13 be an output.
	DDRD = 0xff;

	int i = 0;
    while(1)
    {
		i%=12;
       switch(i){
		case 0:
			PORTD = (1<<PD2); //PD2
			_delay_ms(200);
			break;
		case 1:
			PORTD = (1<<PD3); //PD3
			_delay_ms(200);
			break;
		case 2:
			PORTD = (1<<PD4); //PD4
			_delay_ms(200);
			break;
		case 3:
			PORTD = (1<<PD5); //PD5
			_delay_ms(200);
			break;
		case 4:
			PORTD = (1<<PD6); //PD6
			_delay_ms(200);
			break;
		case 5:
			PORTD = (1<<PD7); //PD7
			_delay_ms(200);
			break;
		case 6:
			PORTB = (1<<PB0); //PB0
			_delay_ms(200);
			break;
		case 7:
			PORTB = (1<<PB1); //PB1
			_delay_ms(200);
			break;
		case 8:
			PORTB = (1<<PB2); //PB2
			_delay_ms(200);
			break;
		case 9:
			PORTB = (1<<PB3); //PB3
			_delay_ms(200);
			break;
		case 10:
			PORTB = (1<<PB4); //PB4
			_delay_ms(200);
			break;
		case 11:
			PORTB = (1<<PB5); //PB5
			_delay_ms(200);
			break;
	}
	i++;
    }
}
