#pragma once
#include "init_setup.h"

#define TRUE 0
#define FALSE 1

#define PREAMBLE_SIZE 1 // 1 Byte
#define CRC_SIZE 4 // 4 Byte
#define SIZE_OF_SIZE 1 // 1 Byte
#define PAYLOAD_SIZE 251 // Maximum: 251 Byte
#define DATA_SIZE (PREAMBLE_SIZE+CRC_SIZE+SIZE_OF_SIZE+PAYLOAD_SIZE) // Preamble(1Byte) + CRC(4Byte) + SIZE(1Byte) + PAYLOAD_SIZE

#define USER_INPUT 0
#define PREAMBLE_FLAG 1
#define CRC_FLAG 2
#define SIZE_FLAG 3
#define PAYLOAD_FLAG 4
#define SEND_COMPLETE 5
#define RECEIVE_COMPLETE 6
#define LAYER3 7
#define IDLE 10

volatile int send_select = USER_INPUT, receive_select = PREAMBLE_FLAG;

uint8_t send_preamble[1] = { 0b01111110 };
uint8_t received_preamble[1] = { 0 };

typedef struct {
	uint8_t crc32[CRC_SIZE];
	uint8_t size[SIZE_OF_SIZE];
	uint8_t payload[PAYLOAD_SIZE];
} frame;

frame* send_frame;
frame* receive_frame;

void make_layer2();
void send_message();
void received_message();
int preamble_check(uint8_t *received_preamble);
int crc_check();
uint8_t* make_crc(uint8_t size, uint8_t* payload);
void make_send_frame(unsigned int SIZE, unsigned int ID, uint8_t* message);
void make_layer3(unsigned int ID, frame *send_frame);
void clear_frame(frame *_frame);

bool read_bit(uint8_t* bitString, int counter);
void write_bit(uint8_t* bitString, int counter, uint8_t bitData);
