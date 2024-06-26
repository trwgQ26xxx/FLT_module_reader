/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#include "hw_leds.h"

#include <avr/io.h>

#define INIT_LEDS		do{ DDRF|= (1 << 1) | (1 << 0); }while(0)


/* Init LEDs */
void Init_LEDs(void)
{
	DETECT_LED_OFF;
	BUSY_LED_OFF;
	INIT_LEDS;
}
