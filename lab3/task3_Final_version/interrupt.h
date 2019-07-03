#pragma once
#include "user_input.c"

//! read data from PIN D2
#define PD2_VOL (PIND & (1<<PIND2))

//! i is used for send counter
volatile int i = 0; 
//! j is used for receive counter
volatile int j = 0;
