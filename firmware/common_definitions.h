

#ifndef _COMMON_DEFINITIONS_H
#define _COMMON_DEFINITIONS_H

/* UART parameters */
#define BAUDRATE						57600UL

/* LED blinking parameters */
#define TMR_PERIOD						20	//ms
#define DETECT_LED_PERIOD				200	//ms

#define DETECT_LED_COUNTER_MAX			(DETECT_LED_PERIOD / TMR_PERIOD)

/* ROM sizes */
#define ROM_NOT_PRESENT					0
#define ROM_IS_PRESENT					1
#define ROM_SIZE_8K						8
#define ROM_SIZE_32K					32
#define ROM_SIZE_64K					64

/* ROM types */
#define ROM_TYPE_A						'A'
#define ROM_TYPE_B						'B'
#define ROM_TYPE_C						'C'

/* Commands */
#define NO_COMMAND						0
#define DETECT_COMMAND					't'
#define READ_COMMAND					'd'

/* ROM chunks parameters */
#define SINGLE_BLOCK_SIZE					512
#define BLOCK_NUMBER_LOW_BANK_MIN_ROM_A		0
#define BLOCK_NUMBER_LOW_BANK_MAX_ROM_A		15
#define BLOCK_NUMBER_LOW_BANK_MIN_ROM_BC	0
#define BLOCK_NUMBER_LOW_BANK_MAX_ROM_BC	63
#define BLOCK_NUMBER_HIGH_BANK_MIN_ROM_BC	64
#define BLOCK_NUMBER_HIGH_BANK_MAX_ROM_BC	127

/* Transmission parameters */
#define TX_BUFFER_MAX_LEN				514

#define RX_CMD_BUFFER_MAX_LEN			10
#define CMD_FIELD_SIZE					1	//CMD (1 byte)
#define CRC_FIELD_SIZE					2	//CRC16 (2 bytes)
#define RX_CMD_MIN_LEN					(CMD_FIELD_SIZE + CRC_FIELD_SIZE)

/* RX frames */
#define CMD_INDEX						0
#define ROM_SEL_INDEX					1
#define BLOCK_NUMBER_INDEX				2

#define DETECT_CMD_LEN					1
#define READ_CMD_LEN					3

/* TX frames */

/* Detect TX frame */
#define DETECT_CMD_RESPONSE_LEN			5
#define DETECT_INDEX					1
#define A_SIZE_INDEX					2
#define B_SIZE_INDEX					3
#define C_SIZE_INDEX					4

/* Read TX frame */
#define READ_CMD_RESPONSE_LEN			514
#define DATA_VALID_INDEX				1
#define DATA_INDEX						2

/* ADC ROM size detecion */
#define NUMBER_OF_ADC_MEASUREMENTS		10
#define DELAY_BETWEEN_ADC_MEASUREMENTS	10	//ms

/* Constant definitions */
#define TRUE							1
#define FALSE							0

#endif
