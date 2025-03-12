/*
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			12.03.2025
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#ifndef _DRV_HW_WDT_H
#define _DRV_HW_WDT_H

void Init_WDT(void);

inline void Poke_WDT(void)
{
	/* Clear WDT */
	asm volatile ("wdr");
}

#endif
