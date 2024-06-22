
#include "hw_uart.h"

/* Init procedure */
void Init_UART(uint32_t baudrate)
{
	uint32_t ubrr = F_CPU / 16 / baudrate - 1;

	/* Init pins */
	PORTD |= (1 << 2);	/* Enable RXD pull-up */

	/* Set baud rate */
	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;

	/* Init registers */
	UCSR1A = 0;

	/* Enable receiver and transmitter */
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);

	/* Set frame format: 8 data bits, no parity, 1stop bit */
	UCSR1C = (0 << USBS1) | (3 << UCSZ10);
}


void TX_single_char(uint8_t single_char)
{
	/* Wait for empty transmit buffer */
	while(!(UCSR1A & (1 << UDRE1)));
	
	/* Put data into buffer, sends the data */
	UDR1 = single_char;
}

