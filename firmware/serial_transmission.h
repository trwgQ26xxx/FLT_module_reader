/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#ifndef _SERIAL_TRANSMISSION_H
#define _SERIAL_TRANSMISSION_H

#include <stdint.h>

void TX_text(char *text);

void Reset_RX_Transmission(void);

uint8_t Process_RX_Transmission(uint8_t *buffer, uint8_t *rx_cmd_len);

void TX_frame(uint8_t *buffer, uint16_t len);

#endif

