/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#include "hw_ext_mem.h"

#include <string.h>

#include <avr/io.h>

#include "../common_definitions.h"

#define EXT_MEM_BASE			0x8000

#define EXT_MEM_MAPPED_SIZE		0x8000	//32k

#define NUMBER_OF_MEM_BLOCKS	(EXT_MEM_MAPPED_SIZE / SINGLE_BLOCK_SIZE)

inline uint8_t Read_ROM_byte(uint16_t adress);

void Init_EXTMEM(void)
{
	/* Enable pull-ups on AD0 - AD7 */
	PORTA |= 0xFF;
	
	/* Enable pull-ups on ALE, #RD & #WR */
	PORTG |= 0x07;
	
	/* Enable pull-ups on A8 - A15 */
	PORTC |= 0xFF;
	
	/* Enable EXT MEM interface, enable wait state */
	MCUCR = (1 << SRE) | (1 << SRW10);
	
	/* Lower section 0x1100-0x7FFF, upper section 0x8000-0xFFFF */
	/* Enable all wait state */
	XMCRA = (1 << SRL2) | (0 << SRL1) | (0 << SRL0) | (1 << SRW01) | (1 << SRW00) | (1 << SRW11);
	
	/* Bus keeper disabled, full 60K addr space available */
	XMCRB = (0 << XMBK) | (0 << XMM2) | (0 << XMM1) | (0 << XMM0);
}

void Read_ROM_block(uint8_t *buffer, uint8_t block_number)
{
	/* Check if range is valid */
	if(block_number < NUMBER_OF_MEM_BLOCKS)
	{
		/* Valid. Read memory */
		for(uint16_t i = 0; i < SINGLE_BLOCK_SIZE; i++)
		{
			buffer[i] = *((uint8_t *)(EXT_MEM_BASE + (SINGLE_BLOCK_SIZE * block_number) + i));
		}
	}
	else
	{
		/* Block number is out of range. Clear buffer */
		memset(buffer, 0xFF, SINGLE_BLOCK_SIZE);
	}
}

uint8_t Check_if_ROM_is_present(uint8_t rom_size)
{
	uint8_t is_empty = FALSE;

	/* for each byte, starting from base addr, to region */
	/* region passed can be 8*1024 or 32*1024 */
	for(uint16_t i = 0; i < (rom_size * 1024); i++)
	{
		/* If EPROM is not present, a low byte of address will be read */
		if(Read_ROM_byte(i) != (uint8_t)(i & 0xFF))
		{
			is_empty = TRUE;
			break;
		}
	}

	return is_empty;
}

uint8_t Check_if_ROM_is_empty(uint8_t rom_size)
{
	uint8_t is_empty = TRUE;

	/* for each byte, starting from base addr, to region */
	/* region passed can be 8*1024 or 32*1024 */
	for(uint16_t i = 0; i < (rom_size * 1024); i++)
	{
		/* If EPROM is not present, a low byte of address will be read */
		if(Read_ROM_byte(i) != EMPTY_ROM_BYTE)
		{
			is_empty = FALSE;
			break;
		}
	}

	return is_empty;
}

inline uint8_t Read_ROM_byte(uint16_t adress)
{
	uint8_t data;

	/* Check if range is valid */
	if(adress < EXT_MEM_MAPPED_SIZE)
	{
		/* Valid. Read memory */
		data = *((uint8_t *)(EXT_MEM_BASE + adress));
	}
	else
	{
		/* Block number is out of range. Clear buffer */
		data = EMPTY_ROM_BYTE;
	}

	return data;
}
