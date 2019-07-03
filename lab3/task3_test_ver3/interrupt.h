#pragma once
#include "user_input.c"

#define PD2_VOL (PIND & (1<<PIND2))

volatile int i = 0, j = 0;