/*
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			12.03.2025
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#include "hw_wdt.h"

#include <avr/io.h>

void Init_WDT(void)
{
	/* Clear WDT */
	asm volatile ("wdr");

	/* Clear reset cause flags */
	MCUCSR = 0;

	/* Start timed sequence by writing WDCE and WDE */
	WDTCR = (1 << WDCE) | (1 << WDE);

	/* Write desired timeout with WDCE cleared, WDE is irrelevant */
	/* 5 sets 0.52s typical timeout @ 5V */
	WDTCR = (0 << WDCE) | (1 << WDE) | (5 << WDP0);
}
