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

#include "ram_test_definitions.h"

#define EXT_MEM_BASE			0x8000

#define EXT_MEM_MAPPED_SIZE		0x8000	//32k

#define NUMBER_OF_MEM_BLOCKS	(EXT_MEM_MAPPED_SIZE / SINGLE_BLOCK_SIZE)

#define RAM_TEST_ADDRESS		0x0000

inline uint8_t Read_ROM_byte(uint16_t address);
inline void Write_RAM_byte(uint16_t address, uint8_t data);

void Set_bit_in_RAM_register(uint16_t address, uint8_t bit);
void Clr_bit_in_RAM_register(uint16_t address, uint8_t bit);

inline void Store_pattern(const uint8_t pattern, uint16_t ram_size);
inline uint8_t Verify_pattern(const uint8_t pattern, uint16_t ram_size);
inline void Store_data(const uint8_t *data, uint16_t ram_size, uint16_t start_addr);
inline uint8_t Verify_data(const uint8_t *data, uint16_t ram_size, uint16_t start_addr);

void Limit_val_to_min(uint8_t min, uint8_t max, uint8_t *val);

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
	
	/* Bus keeper enabled, full 60K addr space available */
	XMCRB = (1 << XMBK) | (0 << XMM2) | (0 << XMM1) | (0 << XMM0);
}

void Read_ROM_block(uint8_t *buffer, uint8_t block_number)
{
	/* Check if range is valid */
	if(block_number < NUMBER_OF_MEM_BLOCKS)
	{
		/* Valid. Read memory */
		for(uint16_t i = 0; i < SINGLE_BLOCK_SIZE; i++)
		{
			volatile uint8_t *ptr = (uint8_t *)(EXT_MEM_BASE + (SINGLE_BLOCK_SIZE * block_number) + i);
			
			buffer[i] = *ptr;
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

uint8_t Check_if_RAM_battery_is_OK(void)
{
	uint8_t battery_is_OK = FALSE;

	uint8_t data_byte = 0x00, complement = 0x00, test_byte = 0x00;

	/* Read data @ any address */
	data_byte = Read_ROM_byte(RAM_TEST_ADDRESS);

	/* Write data complement back to the same address */
	complement = ~data_byte;
	Write_RAM_byte(RAM_TEST_ADDRESS, complement);

	/* Read data at same address again */
	test_byte = Read_ROM_byte(RAM_TEST_ADDRESS);

	/* Is data complement of first read ? */
	if(test_byte == complement)
	{
		/* Yes, battery is OK */
		battery_is_OK = TRUE;
	}
	else
	{
		/* No, battery is low */
		battery_is_OK = FALSE;
	}

	/* Write original data back to same address */
	Write_RAM_byte(RAM_TEST_ADDRESS, data_byte);

	return battery_is_OK;
}

uint8_t Check_if_RAM_memory_is_OK(uint16_t ram_size)
{
	/* Pass 1 - static patterns test */
	for(uint8_t i = 0; i < RAM_TEST_PATTERNS_NUM; i++)
	{
		Store_pattern(ram_test_patterns[i], ram_size);

		if(Verify_pattern(ram_test_patterns[i], ram_size) == FALSE)
		{
			return FALSE;
		}
	}

	/* Final cleanup (0xFF fill) */
	Store_pattern(EMPTY_ROM_BYTE, ram_size);

	/* All pass, RAM is OK */
	return TRUE;
}

void Start_RAM_clock_oscillator(uint16_t base_addr)
{
	/* Start oscillator */
	Clr_bit_in_RAM_register(base_addr+M48T0x_SECONDS_REG_OFFSET, M48T0x_STOP_BIT);
}

void Stop_RAM_clock_oscillator(uint16_t base_addr)
{
	/* Stop oscillator */
	Set_bit_in_RAM_register(base_addr+M48T0x_SECONDS_REG_OFFSET, M48T0x_STOP_BIT);
}

void Reset_RAM_clock(uint16_t base_addr)
{
	/* Stop oscillator */
	Set_bit_in_RAM_register(base_addr+M48T0x_SECONDS_REG_OFFSET, M48T0x_STOP_BIT);

	/* Disable frequency test */
	Clr_bit_in_RAM_register(base_addr+M48T0x_DAY_REG_OFFSET, M48T0x_FREQ_TEST_BIT);

	/* Set WRITE bit, clear READ and SIGN bit, set calibration to 0 */
	Write_RAM_byte(base_addr+M48T0x_CONTROL_REG_OFFSET, M48T0x_WRITE_BIT);

	/* Set year to 0 */
	Write_RAM_byte(base_addr+M48T0x_YEAR_REG_OFFSET, 0x00);

	/* Set month to 1 */
	Write_RAM_byte(base_addr+M48T0x_MONTH_REG_OFFSET, 0x01);

	/* Set date to 1 */
	Write_RAM_byte(base_addr+M48T0x_DATE_REG_OFFSET, 0x01);

	/* Set day to 1, FT bit to 0 */
	Write_RAM_byte(base_addr+M48T0x_DAY_REG_OFFSET, 0x01);

	/* Set hours to 0 */
	Write_RAM_byte(base_addr+M48T0x_HOURS_REG_OFFSET, 0x00);

	/* Set minutes to 0 */
	Write_RAM_byte(base_addr+M48T0x_MINUTES_REG_OFFSET, 0x00);

	/* Set seconds to 0, stop oscillator */
	Write_RAM_byte(base_addr+M48T0x_SECONDS_REG_OFFSET, 0x00 | M48T0x_STOP_BIT);

	/* Clear WRITE, READ and SIGN bits, set calibration to 0 */
	Write_RAM_byte(base_addr+M48T0x_CONTROL_REG_OFFSET, 0x00);
}

void Set_time_in_RAM_clock(uint16_t base_addr, struct time_data *time)
{
	/* Limit variables */
	Limit_val_to_min(0, 99,	&time->year);
	Limit_val_to_min(1, 12,	&time->month);
	Limit_val_to_min(1, 31,	&time->date);
	Limit_val_to_min(1, 7,	&time->day);
	Limit_val_to_min(0, 23,	&time->hour);
	Limit_val_to_min(0, 59,	&time->minute);
	Limit_val_to_min(0, 59,	&time->second);

	/* Set WRITE bit */
	Set_bit_in_RAM_register(base_addr+M48T0x_CONTROL_REG_OFFSET, M48T0x_WRITE_BIT);

	/* Set year */
	Write_RAM_byte(base_addr+M48T0x_YEAR_REG_OFFSET, DEC2BCD(time->year));

	/* Set month */
	Write_RAM_byte(base_addr+M48T0x_MONTH_REG_OFFSET, DEC2BCD(time->month));

	/* Set date */
	Write_RAM_byte(base_addr+M48T0x_DATE_REG_OFFSET, DEC2BCD(time->date));

	/* Set day */
	Write_RAM_byte(base_addr+M48T0x_DAY_REG_OFFSET, DEC2BCD(time->day));

	/* Set hours */
	Write_RAM_byte(base_addr+M48T0x_HOURS_REG_OFFSET, DEC2BCD(time->hour));

	/* Set minutes */
	Write_RAM_byte(base_addr+M48T0x_MINUTES_REG_OFFSET, DEC2BCD(time->minute));

	/* Set seconds */

	/* Check if oscillator is enabled */
	if(Read_ROM_byte(base_addr+M48T0x_SECONDS_REG_OFFSET) & M48T0x_STOP_BIT)
	{
		/* Not enabled, keep STOP bit set */
		Write_RAM_byte(base_addr+M48T0x_SECONDS_REG_OFFSET, DEC2BCD(time->second) | M48T0x_STOP_BIT);
	}
	else
	{
		/* Enabled */
		Write_RAM_byte(base_addr+M48T0x_SECONDS_REG_OFFSET, DEC2BCD(time->second));
	}

	/* Clear WRITE bit, end cycle */
	Clr_bit_in_RAM_register(base_addr+M48T0x_CONTROL_REG_OFFSET, M48T0x_WRITE_BIT);
}

void Get_time_from_RAM_clock(uint16_t base_addr, struct time_data *time)
{
	/* Set READ bit */
	Set_bit_in_RAM_register(base_addr+M48T0x_CONTROL_REG_OFFSET, M48T0x_READ_BIT);

	/* Get year */
	time->year = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_YEAR_REG_OFFSET) & 0xFF);

	/* Get month */
	time->month = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_MONTH_REG_OFFSET) & 0x1F);

	/* Get date */
	time->date = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_DATE_REG_OFFSET) & 0x3F);

	/* Get day */
	time->day = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_DAY_REG_OFFSET) & 0x07);

	/* Get hour */
	time->hour = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_HOURS_REG_OFFSET) & 0x3F);

	/* Get minute */
	time->minute = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_MINUTES_REG_OFFSET) & 0x7F);

	/* Get second */
	time->second = BCD2BIN(Read_ROM_byte(base_addr+M48T0x_SECONDS_REG_OFFSET) & 0x7F);

	/* Clear READ bit */
	Clr_bit_in_RAM_register(base_addr+M48T0x_CONTROL_REG_OFFSET, M48T0x_READ_BIT);
}

inline uint8_t Read_ROM_byte(uint16_t address)
{
	uint8_t data;

	/* Check if range is valid */
	if(address < EXT_MEM_MAPPED_SIZE)
	{
		/* Valid. Read memory */
		volatile uint8_t *ptr = (uint8_t *)(EXT_MEM_BASE + address);

		data = *ptr;
	}
	else
	{
		/* Block number is out of range. Clear buffer */
		data = EMPTY_ROM_BYTE;
	}

	return data;
}

inline void Write_RAM_byte(uint16_t address, uint8_t data)
{
	/* Check if range is valid */
	if(address < EXT_MEM_MAPPED_SIZE)
	{
		/* Valid. Write memory */
		volatile uint8_t *ptr = (uint8_t *)(EXT_MEM_BASE + address);

		*ptr = data;
	}
}

void Set_bit_in_RAM_register(uint16_t address, uint8_t bit)
{
	uint8_t reg = Read_ROM_byte(address);

	reg |= bit;

	Write_RAM_byte(address, reg);
}

void Clr_bit_in_RAM_register(uint16_t address, uint8_t bit)
{
	uint8_t reg = Read_ROM_byte(address);

	reg &= ~bit;

	Write_RAM_byte(address, reg);
}

inline void Store_pattern(const uint8_t pattern, uint16_t ram_size)
{
	for(uint16_t addr = 0; addr < ram_size; addr++)
	{
		Write_RAM_byte(addr, pattern);
	}
}

inline uint8_t Verify_pattern(const uint8_t pattern, uint16_t ram_size)
{
	for(uint16_t addr = 0; addr < ram_size; addr++)
	{
		if(Read_ROM_byte(addr) != pattern)
		{
			return FALSE;
		}
	}

	return TRUE;
}

inline void Store_data(const uint8_t *data, uint16_t ram_size, uint16_t start_addr)
{
	for(uint16_t addr = 0; addr < ram_size; addr++)
	{
		Write_RAM_byte(start_addr+addr, data[addr]);
	}
}

inline uint8_t Verify_data(const uint8_t *data, uint16_t ram_size, uint16_t start_addr)
{
	for(uint16_t addr = 0; addr < ram_size; addr++)
	{
		if(Read_ROM_byte(start_addr+addr) != data[addr])
		{
			return FALSE;
		}
	}

	return TRUE;
}

void Limit_val_to_min(uint8_t min, uint8_t max, uint8_t *val)
{
	if(*val < min || *val > max)
	{
		*val = min;
	}
}
