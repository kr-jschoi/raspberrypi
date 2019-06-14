void print(const char* msg, int size) {
	int k;
	for (k = 0; k < size; k++) uart_transmit(msg[k]);
}

bool read_bit(uint8_t* bitString, int counter) {
	return(bitString[(counter / 8)] & (0b10000000 >> (counter % 8)));
}

void write_bit(uint8_t* bitString, int counter, uint8_t bitData) {
	if (bitData == 1) {
		bitString[(counter / 8)] |= ((0b10000000) >> (counter % 8));
	}
	else {
		bitString[(counter / 8)] &= (0b11111111 - ((0b10000000) >> (counter % 8)));
	}
}
