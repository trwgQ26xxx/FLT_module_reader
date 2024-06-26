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

#define A15_LINE_ADC_CHANNEL	7

/* Init procedure */
void Init_ROMs(void)
{
	INIT_ROM_MODULE_DET;
	SET_A15_TO_HIGH;
	CONFIGURE_A15_AS_OUT;
	DISABLE_ALL_ROMS;
	INIT_ROM_SEL_PORTS;

	/* Configure ADC */
	ADCSRA = (1 << ADEN) | (0x6 << ADPS0);	//ADC Enable, 144kHz clock
	ADCSRB = (0 << ADTS0);					//Free running mode

	ADMUX = (0x1 << REFS0) | (1 << ADLAR) | (A15_LINE_ADC_CHANNEL << MUX0);	//AVcc as reference, left adjust result
}

unsigned char Get_ADC_conversion(void)
{
	ADCSRA |= (1 << ADSC);	//Start conversion

	while(ADCSRA & (1 << ADSC));	//Wait for conversion to finish

	return ADCH;
}

uint8_t Check_B_and_C_ROMs_size(void)
{
	uint8_t ROMs_B_and_C_are_64k = FALSE;

	uint8_t counter, i;

	/* Drive A15 as analog in */
	CONFIGURE_A15_AS_IN;

	/* Take 10 measurements with 8bit ADC, 10ms apart */
	counter = 0;
	for(i = 0; i < NUMBER_OF_ADC_MEASUREMENTS; i++)
	{
		_delay_ms(DELAY_BETWEEN_ADC_MEASUREMENTS);

		if(Get_ADC_conversion() > 128)
		{
			counter++;
		}
	}

	ROMs_B_and_C_are_64k = (counter < (NUMBER_OF_ADC_MEASUREMENTS / 2)) ? TRUE : FALSE;

	/* Drive A15 high */
	SET_A15_TO_HIGH;
	CONFIGURE_A15_AS_OUT;

	return ROMs_B_and_C_are_64k;
}
