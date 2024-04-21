
/*
--- CONNECTIONS ---

LEDs:
PF0 -> DETECT LED
PF1 -> BUSY LED

--- CONNECTIONS ---
*/

#define INIT_LEDS		do{ DDRF|= (1 << 1) | (1 << 0); }while(0)

#define DETECT_LED_ON	do{ PORTF |= (1 << 0);	}while(0)
#define DETECT_LED_OFF	do{ PORTF &= ~(1 << 0);	}while(0)

#define BUSY_LED_ON		do{ PORTF |= (1 << 1);	}while(0)
#define BUSY_LED_OFF	do{ PORTF &= ~(1 << 1);	}while(0)

/* Init LEDs */
void Init_LEDs(void)
{
	DETECT_LED_OFF;
	BUSY_LED_OFF;
	INIT_LEDS;
}
