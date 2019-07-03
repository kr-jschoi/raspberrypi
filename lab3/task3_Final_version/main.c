#include "interrupt.c"

/*
	Output: PB1(send clock), PB2(send data)
	Input: PD2(receive data), PD3(receive clock)
*/

//! This is the main loop.
int main(void){
	// IO port setup
	io_port_setup();

	// Uart initialize
	uart_init();

	// Timer setup
	// period: 10ms
	timer_setup();

	// Pcint setup
	// Use PD3 for Pin change interrupt
	pcint_setup();
	
	// Make layer2
	make_layer2();

	// Global interrupt enable
	sei();
	
	// Main loop
	while (1) {
		// User input mode
		if (send_select == USER_INPUT) {
			send_select = IDLE;
			user_input();
		}

	}
	// free dynamic memories
	free(send_frame);
	free(receive_frame);
}
