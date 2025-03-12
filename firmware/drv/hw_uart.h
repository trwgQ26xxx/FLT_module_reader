/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

/*
--- CONNECTIONS ---

UART 1:
PD2 -> RXD
PD3 -> TXD

--- CONNECTIONS ---
*/

#ifndef _DRV_HW_UART_H
#define _DRV_HW_UART_H

#include <stdint.h>

#include <avr/io.h>

#include "../common_definitions.h"

void Init_UART(uint32_t baudrate);

void TX_single_char(uint8_t single_char);

inline uint8_t Get_RX_char(void)
{
	return UDR1;
}

inline uint8_t Check_is_RX_char_waiting(void)
{
	if((UCSR1A & (1 << RXC1)))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#endif
