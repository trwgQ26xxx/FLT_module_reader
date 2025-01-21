/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#ifndef _DRV_HW_EXT_MEM_H
#define _DRV_HW_EXT_MEM_H

#include <stdint.h>

#define EMPTY_ROM_BYTE			0xFF

struct time_data
{
	uint8_t year;
	uint8_t month;
	uint8_t date;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
};

void Init_EXTMEM(void);

void Read_ROM_block(uint8_t *buffer, uint8_t block_number);

uint8_t Check_if_ROM_is_present(uint8_t rom_size);
uint8_t Check_if_ROM_is_empty(uint8_t rom_size);

uint8_t Check_if_RAM_battery_is_OK(void);
uint8_t Check_if_RAM_memory_is_OK(uint16_t ram_size);

void Start_RAM_clock_oscillator(uint16_t base_addr);
void Stop_RAM_clock_oscillator(uint16_t base_addr);
void Reset_RAM_clock(uint16_t base_addr);

void Set_time_in_RAM_clock(uint16_t base_addr, struct time_data *time);
void Get_time_from_RAM_clock(uint16_t base_addr, struct time_data *time);

#endif
