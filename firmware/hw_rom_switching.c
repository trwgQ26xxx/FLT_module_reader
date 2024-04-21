

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

#define INIT_ROM_SEL_PORTS		do{ DDRD |= (1 << 7) | (1 << 6) | (1 << 5);				}while(0)
#define DISABLE_ALL_ROMS		do{ PORTD &= ~((1 << 7) | (1 << 5)); PORTD |= (1 << 6);	}while(0)
#define ENABLE_A_ROM			do{ PORTD &= ~((1 << 7) | (1 << 6) | (1 << 5));			}while(0)
#define ENABLE_B_ROM			do{ PORTD |= (1 << 7) | (1 << 6) | (1 << 5);			}while(0)
#define ENABLE_C_ROM			do{ PORTD &= ~(1 << 5); PORTD |= (1 << 7) | (1 << 6);	}while(0)

#define INIT_ROM_MODULE_DET		do{ DDRF &= (~(1 << 5)); PORTF |= (1 << 5); 			}while(0)	/* In with pull-up */
#define IS_ROM_MODULE_PRESENT	(!(PINF & (1 << 5)))

#define CONFIGURE_A15_AS_IN		do{ DDRF &= ~(1 << 7); PORTF &= ~(1 << 7);				}while(0)	/* In without pull-up */
#define CONFIGURE_A15_AS_OUT	do{ DDRF |= ~(1 << 7);									}while(0)
#define SET_A15_TO_HIGH			do{ PORTF |= (1 << 7);									}while(0)
#define SET_A15_TO_LOW			do{ PORTF &= ~(1 << 7);									}while(0)
#define A15_LINE_ADC_CHANNEL	7

/* Init procedure */
void Init_ROMs(void)
{
	INIT_ROM_DET;
	CONFIGURE_A15_AS_IN;
	DISABLE_ALL_ROMS;
	INIT_ROM_SEL_PORTS;
}