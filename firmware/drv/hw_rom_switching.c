/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#include "hw_rom_switching.h"

#include <avr/io.h>
#include <util/delay.h>

#include "../common_definitions.h"

#define INIT_ROM_SEL_PORTS		do{ DDRD |= (1 << 7) | (1 << 6) | (1 << 5);				}while(0)
#define INIT_ROM_MODULE_DET		do{ DDRF &= ~(1 << 5); PORTF |= (1 << 5); 				}while(0)	/* In with pull-up */

/* Init procedure */
void Init_ROMs(void)
{
	INIT_ROM_MODULE_DET;

	SET_A15_TO_HIGH;
	CONFIGURE_A15_AS_OUT;

	DISABLE_ALL_ROMS;
	INIT_ROM_SEL_PORTS;

	/* Configure ADC */
	ADCSRA = 0x00;	//ADC Disabled
	ADCSRB = 0x00;	//Free running mode

	ADMUX = (0x1 << REFS0);	//AVcc as reference with external capacitor at AREF pin
}

void Init_unused_pins(void)
{
	SFIOR = (0 << PUD);	//Pull-ups enabled

	/* Port B is unused */
	DDRB = 0x00;
	PORTB = 0xFF;

	/* PD0, PD1 and PD4 are unused */
	DDRD &= ~((1 << 4) | (1 << 1) | (1 << 0));
	PORTD |= ((1 << 4) | (1 << 1) | (1 << 0));

	/* Port E is unused */
	DDRE = 0x00;
	PORTE = 0xFF;

	/* PF2, PF3, PF4 and PF6 are unused */
	DDRF &= ~((1 << 6) | (1 << 4) | (1 << 3) | (1 << 2));
	PORTF |= ((1 << 6) | (1 << 4) | (1 << 3) | (1 << 2));

	/* PG3 and PG4 are unused */
	DDRG &= ~((1 << 4) | (1 << 3));
	PORTG |= ((1 << 4) | (1 << 3));
}
