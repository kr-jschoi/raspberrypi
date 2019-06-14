#pragma once

#define TRUE 0
#define FALSE 1

#define PREAMBLE_SIZE 1 // 1 Byte
#define CRC_SIZE 4 // 4 Byte
#define SIZE_OF_SIZE 1 // 1 Byte
#define PAYLOAD_SIZE 251 // 4 Byte(Maximum: 251 Byte)
#define DATA_SIZE (PREAMBLE_SIZE+CRC_SIZE+SIZE_OF_SIZE+PAYLOAD_SIZE) // Preamble(1Byte) + CRC(4Byte) + SIZE(1Byte) + PAYLOAD_SIZE

typedef struct {
	uint8_t crc32[CRC_SIZE];
	uint8_t size[SIZE_OF_SIZE];
	uint8_t payload[PAYLOAD_SIZE];
} frame;

uint8_t send_preamble[1] = { 0b01111110 };
uint8_t received_preamble[1] = {0};

volatile bool send_complete = 0; // set 1 after sending frame
volatile bool receive_complete = 0; // set 1 after receiving frame

frame* send_frame;
frame* receive_frame;

void make_layer2();
void send_message();
void received_message();
int preamble_check(uint8_t *received_preamble);
int crc_check();
uint8_t* make_crc(uint8_t size, uint8_t* payload);
void make_send_frame();
void make_layer3(frame *send_frame);

bool read_bit(uint8_t* bitString, int counter);
void write_bit(uint8_t* bitString, int counter, uint8_t bitData);
