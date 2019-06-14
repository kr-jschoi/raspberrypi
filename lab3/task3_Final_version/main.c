#include "interrupt.c"

/*******************************************MAIN**********************************************/
int main(void){
	io_port_setup(); // Output: PB1,PB2 / Input Port: PD2
	uart_init();
	timer_setup(); // period: 10ms
	pcint_setup(); // use PD3 PORT
	
	make_layer2();
	sei(); // grobal interrupt enable
	
	while (1) {
		if (send_select == USER_INPUT) {
			send_select = IDLE;
			user_input();
		}

	}
	free(send_frame); // remove memory
	free(receive_frame); // remove memory
}
/*********************************************************************************************/
