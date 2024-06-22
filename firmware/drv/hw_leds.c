

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
