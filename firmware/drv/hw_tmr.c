/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 7.3.0
*/

#include "hw_tmr.h"

#include <avr/io.h>

void Init_TMR(uint16_t time)
{
	unsigned int period = (((F_CPU / 1024) * time) / 1000) - 1;

	TCCR1A = 0;
	TCCR1B = (5 << CS10) | (1 << WGM12);	// 1024 prescaler
	TCCR1C = 0;

	TCNT1H = 0;	// Clear Timer 1
	TCNT1L = 0;

	OCR1AH = (unsigned char)(period >> 8);
	OCR1AL = (unsigned char)(period);

	TIFR = (1 << OCF1A);	//Clear OCA Match IRQ flag
	TIMSK |= (1 << OCIE1A);	//Enable OCA Match IRQ
}
