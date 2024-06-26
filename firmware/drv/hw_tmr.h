/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#ifndef _DRV_HW_TMR_H
#define _DRV_HW_TMR_H

#include <stdint.h>

#include <avr/interrupt.h>

#define ISR_TIMER_VECTOR	TIMER1_COMPA_vect

void Init_TMR(uint16_t time);

#endif
