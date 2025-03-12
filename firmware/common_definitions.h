/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 7.3.0
*/

#ifndef _COMMON_DEFINITIONS_H
#define _COMMON_DEFINITIONS_H

/* UART parameters */
#define BAUDRATE							115200UL

/* LED blinking parameters */
#define TMR_PERIOD							20	//ms
#define DETECT_LED_PERIOD					200	//ms

#define DETECT_LED_COUNTER_MAX				(DETECT_LED_PERIOD / TMR_PERIOD)

/* ROM sizes */
#define ROM_NOT_PRESENT						0
#define ROM_IS_PRESENT						1
#define RAM_SIZE_2K							2
#define ROM_SIZE_8K							8
#define ROM_SIZE_32K						32
#define ROM_SIZE_64K						64

/* ROM types */
#define ROM_TYPE_A							'A'
#define ROM_TYPE_B							'B'
#define ROM_TYPE_C							'C'

/* Commands */
#define NO_COMMAND							0
#define DETECT_COMMAND						't'
#define READ_COMMAND						'd'
#define RAMTEST_COMMAND						'r'
#define CONTROL_RAM_CLOCK_COMMAND			'c'
#define GET_RAM_CLOCK_COMMAND				'g'
#define SET_RAM_CLOCK_COMMAND				's'

/* RAM types for ram-test */
#define M48Z02_RAM_TYPE						'z'
#define M48T02_RAM_TYPE						't'
#define M48Z08_RAM_TYPE						'Z'
#define M48T08_RAM_TYPE						'T'
#define S62256_RAM_TYPE						'6'

#define START_CLOCK_OSCILLATOR				'e'
#define STOP_CLOCK_OSCILLATOR				'd'
#define RESET_CLOCK							'r'

/* ROM chunks parameters */
#define SINGLE_BLOCK_SIZE					512
#define BLOCK_NUMBER_LOW_BANK_MIN_ROM_A		0
#define BLOCK_NUMBER_LOW_BANK_MAX_ROM_A		15
#define BLOCK_NUMBER_LOW_BANK_MIN_ROM_BC	0
#define BLOCK_NUMBER_LOW_BANK_MAX_ROM_BC	63
#define BLOCK_NUMBER_HIGH_BANK_MIN_ROM_BC	64
#define BLOCK_NUMBER_HIGH_BANK_MAX_ROM_BC	127

/* Transmission parameters */
#define TX_BUFFER_MAX_LEN					514

#define RX_CMD_BUFFER_MAX_LEN				16
#define CMD_FIELD_SIZE						1	//CMD (1 byte)
#define CRC_FIELD_SIZE						2	//CRC16 (2 bytes)
#define RX_CMD_MIN_LEN						(CMD_FIELD_SIZE + CRC_FIELD_SIZE)

/* RX frames */
#define CMD_INDEX							0
#define ROM_SEL_INDEX						1
#define RAM_TYPE_INDEX						1
#define BLOCK_NUMBER_INDEX					2
#define RAM_CLOCK_CMD_INDEX					2
#define RAM_CLOCK_SET_DATA_INDEX			2

#define DETECT_CMD_LEN						1
#define READ_CMD_LEN						3
#define RAMTEST_CMD_LEN						2
#define CONTROL_RAM_CLOCK_CMD_LEN			3
#define GET_RAM_CLOCK_CMD_LEN				2
#define SET_RAM_CLOCK_CMD_LEN				9

/* TX frames */

/* Detect TX frame */
#define DETECT_CMD_RESPONSE_LEN				5
#define DETECT_INDEX						1
#define A_SIZE_INDEX						2
#define B_SIZE_INDEX						3
#define C_SIZE_INDEX						4

/* Read TX frame */
#define READ_CMD_RESPONSE_LEN				514
#define DATA_VALID_INDEX					1
#define DATA_INDEX							2

/* RAM test TX frame */
#define RAMTEST_CMD_RESPONSE_LEN			4
#define RAM_BATTERY_IS_OK_INDEX				2
#define RAM_MEMORY_IS_OK_INDEX				3

/* Control RAM clock command TX frame */
#define CONTROL_RAM_CLOCK_CMD_RESPONSE_LEN	3
#define EXECUTION_OK_INDEX					2

/* Get RAM clock data TX frame */
#define GET_RAM_CLOCK_CMD_RESPONSE_LEN		10
#define CLOCK_DATA_VALID_INDEX				2
#define CLOCK_DATA_GET_DATA_INDEX			3

/* Set RAM clock data TX frame */
#define SET_RAM_CLOCK_CMD_RESPONSE_LEN		3

/* RAM sizes for RAM test */
#define M48Z02_RAM_SIZE						0x800
#define M48T02_RAM_SIZE						0x7F8
#define M48Z08_RAM_SIZE						0x2000
#define M48T08_RAM_SIZE						0x1FF8
#define S62256_RAM_SIZE						0x8000

/* Constant definitions */
#define TRUE								1
#define FALSE								0

#endif
