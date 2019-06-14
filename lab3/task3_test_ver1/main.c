#include "init_setup.h"
#include "interrupt.c"

/*******************************************MAIN**********************************************/
int main(void){
	io_port_setup(); // Output: PB1,PB2 / Input Port: PD2
	uart_init();
	timer_setup(); // period: 10ms
	pcint_setup(); // use PD3 PORT
	
	make_layer2();
	make_send_frame();
	sei(); // grobal interrupt enable
	
	while (1) {
		if (send_complete == 1) {
			TIMSK1 &= ~(1 << OCIE1B); // Timer 1B Interrupt disable
		//	send_message(); 
		}	
		if (receive_complete == 1) {
			TIMSK1 &= ~(1 << OCIE1A); // Timer 1A Interrupt disable
			received_message();
		}
	}
	free(send_frame); // delete memory
	free(receive_frame); // delete memory
}
/*********************************************************************************************/
