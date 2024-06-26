/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

/*
--- CONNECTIONS ---

LEDs:
PF0 -> DETECT LED
PF1 -> BUSY LED

--- CONNECTIONS ---
*/

#ifndef _DRV_HW_LEDS_H
#define _DRV_HW_LEDS_H

#include <stdint.h>

#define DETECT_LED_ON		do{ PORTF |= (1 << 0);	}while(0)
#define DETECT_LED_OFF		do{ PORTF &= ~(1 << 0);	}while(0)
#define DETECT_LED_TOGGLE	do{ PORTF ^= (1 << 0);	}while(0)

#define BUSY_LED_ON			do{ PORTF |= (1 << 1);	}while(0)
#define BUSY_LED_OFF		do{ PORTF &= ~(1 << 1);	}while(0)
#define BUSY_LED_TOGGLE		do{ PORTF ^= (1 << 1);	}while(0)

/* Init procedure */
void Init_LEDs(void);

#endif
