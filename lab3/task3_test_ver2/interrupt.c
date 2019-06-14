#pragma once
#include "interrupt.h"

/*[1]Physical layer: Clock signal(send:PB1->PD3)*/
ISR(TIMER1_COMPA_vect) {
	PORTB ^= (1 << PB1); // Clock signal
}

/*[2]Data Link layer: send data(send:PB2->PD2)*/
ISR(TIMER1_COMPB_vect) {
	if (send_select == PREAMBLE_FLAG) { // send Preemble
		if (read_bit(send_preamble, i) == 1) { PORTB |= (1 << PINB2); } // send preamble(PB2->PD2)
		else { PORTB &= ~(1 << PINB2); }
		i++;
		if (i == (PREAMBLE_SIZE * 8)) { i = 0; send_select = CRC_FLAG; }
	}
	else if (send_select == CRC_FLAG) { // send CRC
		if (read_bit(send_frame->crc32, i) == 1) { PORTB |= (1 << PINB2); } // send crc(PB2->PD2)
		else { PORTB &= ~(1 << PINB2); }
		i++;
		if (i == (CRC_SIZE * 8)) { i = 0; send_select = SIZE_FLAG; }
	}
	else if (send_select == SIZE_FLAG) { // send SIZE
		if (read_bit(send_frame->size, i) == 1) { PORTB |= (1 << PINB2); } // send size(PB2->PD2)
		else { PORTB &= ~(1 << PINB2); }
		i++;
		if (i == (SIZE_OF_SIZE * 8)) { i = 0; send_select = PAYLOAD_FLAG; }
	}
	else if (send_select == PAYLOAD_FLAG) { // send PAYLOAD
		if (read_bit(send_frame->payload, i) == 1) { PORTB |= (1 << PINB2); } // send payload(PB2->PD2)
		else { PORTB &= ~(1 << PINB2); }
		i++;
		if (i == ((send_frame->size[0]) * 8)) { i = 0; send_select = SEND_COMPLETE; }
	}
	else if (send_select == SEND_COMPLETE) { // send complete
		send_message(); // show send message
	}
}

/*[1]Physical layer: Clock signal(recieve:PB1->PD3)*/
/*[2]Data Link layer: read data(receive:PD2<-PB2)*/
ISR(PCINT2_vect) {
	if (receive_select == PREAMBLE_FLAG) { // receive preamble
		if (j < (PREAMBLE_SIZE * 8)) {
			if (PD2_VOL) { write_bit(received_preamble, j, 1); } // read data(PD2<-PB2)
			else { write_bit(received_preamble, j, 0); }
			j++;
		}
		if (j == (PREAMBLE_SIZE * 8)){
			if (preamble_check(received_preamble) == TRUE) { j = 0; receive_select = CRC_FLAG;} // Preamble detected
		j++;
		}
		else if(j>8){	
			if (PD2_VOL) { received_preamble[0] <<= 1; write_bit(received_preamble, 7, 1); } // receive preamble until "01111110"
			else { received_preamble[0] <<= 1; write_bit(received_preamble, 7, 0); }
		
			if (preamble_check(received_preamble) == TRUE) { j = 0; receive_select = CRC_FLAG;} // Preamble detected
		}
	}
	else if (receive_select == CRC_FLAG) { // receive CRC
		if (PD2_VOL) { write_bit(receive_frame->crc32, j, 1); } // read data(PD2<-PB2)
		else { write_bit(receive_frame->crc32, j, 0); }
		j++;
		if (j == (CRC_SIZE * 8)) { j = 0; receive_select = SIZE_FLAG; }
	}
	else if (receive_select == SIZE_FLAG) { // receive SIZE
		if (PD2_VOL) { write_bit(receive_frame->size, j, 1); } // read data(PD2<-PB2)
		else { write_bit(receive_frame->size, j, 0); }
		j++;
		if (j == (SIZE_OF_SIZE * 8)) { j = 0; receive_select = PAYLOAD_FLAG; }
	}
	else if (receive_select == PAYLOAD_FLAG) { // receive PAYLOAD
		if (PD2_VOL) { write_bit(receive_frame->payload, j, 1); } // read data(PD2<-PB2)
		else { write_bit(receive_frame->payload, j, 0); }
		j++;
		if (j == ((receive_frame->size[0]) * 8)) { j = 0; receive_select = LAYER3; }
	}
	else if (receive_select == LAYER3) { // ID Check
		ID_check(send_frame, receive_frame);
	}
	else if (receive_select == RECEIVE_COMPLETE) { // recieve complete
		received_message(); // show received massage and then enable to receive data mode
	}
}
