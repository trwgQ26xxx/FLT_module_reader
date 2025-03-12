/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#include "serial_transmission.h"

#include "drv/hw_uart.h"

#include "common_definitions.h"

#include <stdio.h>

#define SOF_CHAR		0xFA
#define EOF_CHAR		0xFB
#define ESC_CHAR		0xFC

volatile uint8_t rx_buffer_pos = 0;
volatile uint8_t esc_char_received = FALSE;

inline uint16_t Compute_CRC16(uint8_t *data, uint16_t length);
inline void Encode_and_send_bytes(uint8_t *buffer, uint16_t len);
inline uint8_t Verify_frame_CRC(uint8_t *buffer);

void Reset_RX_Transmission(void)
{
	/* Clear flags */
	rx_buffer_pos = 0;
	esc_char_received = 0;
}

unsigned char Process_RX_Transmission(uint8_t *buffer, uint8_t *rx_cmd_len)
{
	uint8_t valid_frame_was_received = FALSE;
	uint8_t character;

	while(Check_is_RX_char_waiting() == TRUE)
	{
		/* Get character */
		character = Get_RX_char();

		switch(character)
		{
			case SOF_CHAR:
				/* Start of frame char */
			
				/* Start receiving */
				Reset_RX_Transmission();

				break;
			
			
			case EOF_CHAR:
				/* End of frame char */

				/* Check for minimal length */
				if(rx_buffer_pos >= RX_CMD_MIN_LEN)
				{
					/* If OK, check CRC */
					valid_frame_was_received = Verify_frame_CRC(buffer);
				}
			
				break;
			
			case ESC_CHAR:
				/* Escape char */
			
				/* Mark that ESC character was received */
				esc_char_received = TRUE;
			
				break;
				
				
			default:
				/* Any other char */
				
				/* Check if esc char was received */
				if(esc_char_received == FALSE)
				{
					/* If no, store char as is */				
					buffer[rx_buffer_pos++] = character;
				}
				else
				{
					/* If so, store negated char */
					buffer[rx_buffer_pos++] = ~character;
					
					/* Clear esc char flag */
					esc_char_received = FALSE;
				}
				
				/* Check for buffer overflow */
				if(rx_buffer_pos >= RX_CMD_BUFFER_MAX_LEN)
				{
					Reset_RX_Transmission();
				}

				break;
		}
	
		if(valid_frame_was_received == TRUE)
		{
			break;
		}
	}

	if(valid_frame_was_received == TRUE)
	{
		*rx_cmd_len = rx_buffer_pos - CRC_FIELD_SIZE;

		return buffer[CMD_INDEX];
	}
	else
	{
		*rx_cmd_len = 0;

		return NO_COMMAND;
	}
}

void TX_text(char *text)
{
	uint32_t index = 0;

	while(text[index] != 0)
	{
		TX_single_char(text[index]);
		index++;
	}

	TX_single_char('\r');
	TX_single_char('\n');
}

void TX_frame(uint8_t *buffer, uint16_t len)
{
	uint16_t crc;
	uint8_t crc_buffer[CRC_FIELD_SIZE];

	/* Compute CRC */
	crc = Compute_CRC16(buffer, len);

	/* Pack CRC into buffer */
	crc_buffer[0] = (uint8_t)(crc >> 8);
	crc_buffer[1] = (uint8_t)(crc);
	
	/* Send start of frame */
	TX_single_char(SOF_CHAR);
	
	/* Send frame */
	Encode_and_send_bytes(buffer, len);

	/* Send CRC */
	Encode_and_send_bytes(crc_buffer, CRC_FIELD_SIZE);

	/* Send end of frame */
	TX_single_char(EOF_CHAR);
}

inline void Encode_and_send_bytes(uint8_t *buffer, uint16_t len)
{
	/* Encode and send each byte */
	for(uint16_t i = 0; i < len; i++)
	{
		/* Check if buffer contains special char */
		if((buffer[i] == ESC_CHAR) || (buffer[i] == SOF_CHAR) || (buffer[i] == EOF_CHAR))
		{
			/* If so, send first ESC_CHAR */
			TX_single_char(ESC_CHAR);
			
			/* And negated byte next */
			TX_single_char(~buffer[i]);
		}
		else
		{
			/* If not, send as is */
			TX_single_char(buffer[i]);
		}
	}
}

inline uint16_t Compute_CRC16(uint8_t *data, uint16_t length)
{
	uint16_t generator = 0x1021; /* divisor is 16bit */
	uint16_t crc = 0; /* CRC value is 16bit */
	uint16_t i, j;

    for(i = 0; i < length; i++)
    {
        crc ^= (unsigned int)(data[i] << 8); /* move byte into MSB of 16bit CRC */

        for (j = 0; j < 8; j++)
        {
            if ((crc & 0x8000) != 0) /* test for MSB = bit 15 */
            {
                crc = (unsigned int)((crc << 1) ^ generator);
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

inline uint8_t Verify_frame_CRC(uint8_t *buffer)
{
	/* Get frame CRC */
	uint16_t expected_CRC = (buffer[rx_buffer_pos - 2] << 8) | (buffer[rx_buffer_pos - 1]);

	if(expected_CRC == Compute_CRC16(buffer, rx_buffer_pos - CRC_FIELD_SIZE))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
