#pragma once
#include "layer3.c"

#define PD2_VOL (PIND & (1<<PIND2))

volatile int i = 0, j = 0;