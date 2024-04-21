

/*
--- CONNECTIONS ---

UART:
PD2 -> RXD
PD3 -> TXD

--- CONNECTIONS ---
*/

#define BAUDRATE	57600UL

/* Init procedure */
void Init_UART(void)
{
	PORTD |= (1 << 2);	/* Enable RXD pull-up */
	
}