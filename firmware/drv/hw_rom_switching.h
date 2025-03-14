/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 7.3.0
*/

/*
--- CONNECTIONS ---

BANK SWITCHING:
PD5 -> B
PD6 -> #A
PD7 -> B + C

ROM MODULE DETECTION:
PF5 -> #ROM_DET

A15 LINE MANAGEMENT:
PF7 -> A15

--- CONNECTIONS ---
*/

#ifndef _DRV_HW_ROM_SWITCHING_H
#define _DRV_HW_ROM_SWITCHING_H

#include <stdint.h>

/* ROM selection */
#define DISABLE_ALL_ROMS		do{ PORTD &= ~((1 << 7) | (1 << 5)); PORTD |= (1 << 6);	}while(0)
#define ENABLE_A_ROM			do{ PORTD &= ~((1 << 7) | (1 << 6) | (1 << 5));			}while(0)
#define ENABLE_B_ROM			do{ PORTD |= (1 << 7) | (1 << 6) | (1 << 5);			}while(0)
#define ENABLE_C_ROM			do{ PORTD &= ~(1 << 5); PORTD |= (1 << 7) | (1 << 6);	}while(0)

/* ROM module detection */
#define IS_ROM_MODULE_PRESENT	(!(PINF & (1 << 5)))

/* A15 pin switching */
#define CONFIGURE_A15_AS_OUT	do{ DDRF |= (1 << 7);									}while(0)
#define SET_A15_TO_HIGH			do{ PORTF |= (1 << 7);									}while(0)
#define SET_A15_TO_LOW			do{ PORTF &= ~(1 << 7);									}while(0)

/* Initialize function */
void Init_ROMs(void);

void Init_unused_pins(void);

#endif
