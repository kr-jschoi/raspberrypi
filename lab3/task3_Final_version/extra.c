//! This is the function to print out the message
/*!
   \param msg: message that wants to print
   \param size: size of message
   \return void
*/
void print(const char* msg, int size) {
	int k;
	for (k = 0; k < size; k++) uart_transmit(msg[k]);
}

//! This is the function to read bit from MSB
/*!
   \param bitString: bit data to read
   \param counter: shift counter
   \return void
*/
bool read_bit(uint8_t* bitString, int counter) {
	return(bitString[(counter / 8)] & (0b10000000 >> (counter % 8)));
}

//! This is the function to write bit from MSB
/*!
   \param bitString: bit data to write
   \param counter: shift counter
   \param bitData: distinguish whether the data to write is 1 or 0
   \return void
*/
void write_bit(uint8_t* bitString, int counter, uint8_t bitData) {
	if (bitData == 1) {
		bitString[(counter / 8)] |= ((0b10000000) >> (counter % 8));
	}
	else {
		bitString[(counter / 8)] &= (0b11111111 - ((0b10000000) >> (counter % 8)));
	}
}
