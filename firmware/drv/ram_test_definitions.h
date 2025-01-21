/*
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			18.01.2025
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#ifndef _DRV_RAM_TEST_DEFINITIONS_H
#define _DRV_RAM_TEST_DEFINITIONS_H

#include <stdint.h>

#define M48T0x_YEAR_REG_OFFSET		0x7
#define M48T0x_MONTH_REG_OFFSET		0x6
#define M48T0x_DATE_REG_OFFSET		0x5
#define M48T0x_DAY_REG_OFFSET		0x4
#define M48T0x_HOURS_REG_OFFSET		0x3
#define M48T0x_MINUTES_REG_OFFSET	0x2
#define M48T0x_SECONDS_REG_OFFSET	0x1
#define M48T0x_CONTROL_REG_OFFSET	0x0

#define M48T0x_FREQ_TEST_BIT		0x40	//in DAY reg
#define M48T0x_SIGN_BIT				0x20	//in CONTROL reg
#define M48T0x_READ_BIT				0x40	//in CONTROL reg
#define M48T0x_WRITE_BIT			0x80	//in CONTROL reg
#define M48T0x_STOP_BIT				0x80	//in SECONDS reg

#define M48T0x_CALIB_MASK			0x1F	//in CONTROL reg

#define RAM_TEST_PATTERNS_NUM 		4

const uint8_t ram_test_patterns[RAM_TEST_PATTERNS_NUM] = {0x00, 0xFF, 0x55, 0xAA};

#define DEC2BCD(in) (uint8_t)(((in / 10) << 4) | (in % 10))
#define BCD2BIN(in) (uint8_t)((((in & 0xF0) >> 4) * 10) + (in & 0x0F))

#endif
