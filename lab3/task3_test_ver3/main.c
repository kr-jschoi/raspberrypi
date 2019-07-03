#include "interrupt.c"

/*******************************************MAIN**********************************************/
int main(void){
	/*
	Output: PB1(send clock), PB2(send data)
	Input: PD2(receive data), PD3(receive clock) 
	*/
	io_port_setup();
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
