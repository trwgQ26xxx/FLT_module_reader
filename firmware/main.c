/* 
Project name:	FLT/ART/CU1 ROM Reader
Author: 		trwgQ26xxx
Date:			26.06.2024
Target MCUs:	ATmega64/ATmega64L/ATmega64A/ATmega128/ATmega128L/ATmega128A
Compiler:		AVR-GCC version 5.4.0 (32-bit)
*/

#include <stdint.h>
#include <string.h>

#include <util/delay.h>

#include "common_definitions.h"

#include "drv/hw_ext_mem.h"
#include "drv/hw_uart.h"
#include "drv/hw_rom_switching.h"
#include "drv/hw_leds.h"
#include "drv/hw_tmr.h"

#include "serial_transmission.h"


volatile uint8_t module_detection_was_performed 	= FALSE;
volatile uint8_t module_is_connected 				= FALSE;

volatile uint8_t ROM_A_is_present = ROM_NOT_PRESENT;
volatile uint8_t ROM_B_is_present = ROM_NOT_PRESENT;
volatile uint8_t ROM_C_is_present = ROM_NOT_PRESENT;

volatile uint8_t RAM_battery_is_OK = FALSE;
volatile uint8_t RAM_memory_is_OK = FALSE;

static uint8_t rx_cmd_buffer[RX_CMD_BUFFER_MAX_LEN], rx_cmd_len = 0;
static uint8_t tx_response_buffer[TX_BUFFER_MAX_LEN];

inline uint8_t Select_correct_bank_for_A_ROM(uint8_t block_number, uint8_t *block_to_read);
inline uint8_t Select_correct_bank_for_B_and_C_ROMs(uint8_t block_number, uint8_t *block_to_read);
inline void Clear_ROM_info(void);
inline uint8_t Check_8K_ROM(void);
inline uint8_t Check_32K_ROM(void);
inline uint8_t Check_64K_ROMs(void);

void RomDetection(void);
void RomRead(void);

void RamTest(void);

void ControlRamClock(void);
void GetRamClock(void);
void SetRamClock(void);

int main(void)
{
	/* Initialize peripherals */
    Init_EXTMEM();
    Init_LEDs();
    Init_ROMs();
    Init_UART(BAUDRATE);
    Init_TMR(TMR_PERIOD);
    Init_unused_pins();

    /* Enable IRQs */
    sei();

    /* Prepare UART reception */
    Reset_RX_Transmission();

    /* Clear variables */
    Clear_ROM_info();

    /* Disable auto detect */
	SET_A15_TO_HIGH;
	CONFIGURE_A15_AS_OUT;

    /* Main loop */
    while(1)
    {
		switch(Process_RX_Transmission(rx_cmd_buffer, &rx_cmd_len))
		{
		case DETECT_COMMAND:
			RomDetection();
			break;

		case READ_COMMAND:
			RomRead();
			break;

		case RAMTEST_COMMAND:
			RamTest();
			break;

		case CONTROL_RAM_CLOCK_COMMAND:
			ControlRamClock();
			break;

		case GET_RAM_CLOCK_COMMAND:
			GetRamClock();
			break;

		case SET_RAM_CLOCK_COMMAND:
			SetRamClock();
			break;

		default:
			/* Nothing */
			break;
		}
    }

    /* Program should never go here */
    return 0;
}

/* ROM_DETECTION_CODE */
void RomDetection(void)
{
	/* Check command length */
	if(rx_cmd_len == DETECT_CMD_LEN)
	{
		BUSY_LED_ON;

		/* Perform detection only if module is connected */
		if(module_is_connected == TRUE)
		{
			SET_A15_TO_HIGH;
			CONFIGURE_A15_AS_OUT;

			/* Check ROM A */
			ENABLE_A_ROM;
			ROM_A_is_present = Check_8K_ROM();

			/* Check ROM B */
			ENABLE_B_ROM;
			ROM_B_is_present = Check_64K_ROMs();

			/* Check ROM C */
			ENABLE_C_ROM;
			ROM_C_is_present = Check_64K_ROMs();

			SET_A15_TO_HIGH;
			DISABLE_ALL_ROMS;

			module_detection_was_performed = 1;
		}

		/* Compose response */
		tx_response_buffer[CMD_INDEX]		= DETECT_COMMAND;
		tx_response_buffer[DETECT_INDEX]	= module_is_connected;
		tx_response_buffer[A_SIZE_INDEX]	= ROM_A_is_present;
		tx_response_buffer[B_SIZE_INDEX]	= ROM_B_is_present;
		tx_response_buffer[C_SIZE_INDEX]	= ROM_C_is_present;

		/* Send response to PC */
		TX_frame(tx_response_buffer, DETECT_CMD_RESPONSE_LEN);

		BUSY_LED_OFF;
	}

	return;
}

/* READ_ROM_CODE */
void RomRead(void)
{
	uint8_t data_valid = TRUE;
	uint8_t block_to_read = 0;

	/* Check command length */
	if(rx_cmd_len == READ_CMD_LEN)
	{
		BUSY_LED_ON;

		uint8_t rom_sel 		= rx_cmd_buffer[ROM_SEL_INDEX];
		uint8_t block_number 	= rx_cmd_buffer[BLOCK_NUMBER_INDEX];

		if((module_is_connected == TRUE) && (module_detection_was_performed == TRUE))
		{
			/* Select correct ROM */
			switch(rom_sel)
			{
			case ROM_TYPE_A:
				data_valid = Select_correct_bank_for_A_ROM(block_number, &block_to_read);

				ENABLE_A_ROM;
				break;

			case ROM_TYPE_B:
				data_valid = Select_correct_bank_for_B_and_C_ROMs(block_number, &block_to_read);

				ENABLE_B_ROM;
				break;

			case ROM_TYPE_C:
				data_valid = Select_correct_bank_for_B_and_C_ROMs(block_number, &block_to_read);

				ENABLE_C_ROM;
				break;

			default:
				/* Data invalid */
				data_valid = FALSE;

				/* Wrong ROM */
				DISABLE_ALL_ROMS;

				break;
			}

			/* Read data */
			Read_ROM_block(&tx_response_buffer[DATA_INDEX], block_to_read);
		}
		else
		{
			data_valid = FALSE;
		}

		/* Disable ROMs */
		DISABLE_ALL_ROMS;
		SET_A15_TO_HIGH;

		/* Compose response */
		tx_response_buffer[CMD_INDEX] 			= READ_COMMAND;
		tx_response_buffer[DATA_VALID_INDEX]	= data_valid;

		if(data_valid == FALSE)
		{
			/* Clear buffer if data is not valid */
			memset(&tx_response_buffer[DATA_INDEX], 0xFF, SINGLE_BLOCK_SIZE);
		}

		/* Send response to PC */
		TX_frame(tx_response_buffer, READ_CMD_RESPONSE_LEN);

		BUSY_LED_OFF;
	}

	return;
}

/* RAM_TEST_CODE */
void RamTest(void)
{
	/* Check command length */
	if(rx_cmd_len == RAMTEST_CMD_LEN)
	{
		BUSY_LED_ON;

		uint8_t ram_type = rx_cmd_buffer[RAM_TYPE_INDEX];

		/* Perform test only if module is connected */
		if(module_is_connected == TRUE)
		{
			/* Select correct RAM */
			switch(ram_type)
			{
				case M48T02_RAM_TYPE:

					RAM_battery_is_OK = Check_if_RAM_battery_is_OK();
					RAM_memory_is_OK = Check_if_RAM_memory_is_OK(M48T02_RAM_SIZE);

					break;

				case M48Z02_RAM_TYPE:

					RAM_battery_is_OK = Check_if_RAM_battery_is_OK();
					RAM_memory_is_OK = Check_if_RAM_memory_is_OK(M48Z02_RAM_SIZE);

					break;

				case M48T08_RAM_TYPE:

					RAM_battery_is_OK = Check_if_RAM_battery_is_OK();
					RAM_memory_is_OK = Check_if_RAM_memory_is_OK(M48T08_RAM_SIZE);

					break;

				case M48Z08_RAM_TYPE:

					RAM_battery_is_OK = Check_if_RAM_battery_is_OK();
					RAM_memory_is_OK = Check_if_RAM_memory_is_OK(M48Z08_RAM_SIZE);

					break;

				case S62256_RAM_TYPE:

					/* That RAM type is not battery-backed */
					RAM_battery_is_OK = FALSE;
					RAM_memory_is_OK = Check_if_RAM_memory_is_OK(S62256_RAM_SIZE);

					break;

				default:

					/* Unsupported RAM type */
					RAM_battery_is_OK = FALSE;
					RAM_memory_is_OK = FALSE;

					break;
			}

			module_detection_was_performed = 1;
		}

		/* Compose response */
		tx_response_buffer[CMD_INDEX]				= RAMTEST_COMMAND;
		tx_response_buffer[DETECT_INDEX]			= module_is_connected;
		tx_response_buffer[RAM_BATTERY_IS_OK_INDEX]	= RAM_battery_is_OK;
		tx_response_buffer[RAM_MEMORY_IS_OK_INDEX]	= RAM_memory_is_OK;

		/* Send response to PC */
		TX_frame(tx_response_buffer, RAMTEST_CMD_RESPONSE_LEN);

		BUSY_LED_OFF;
	}

	return;
}

void ControlRamClock(void)
{
	uint8_t execution_OK = TRUE;

	/* Check command length */
	if(rx_cmd_len == CONTROL_RAM_CLOCK_CMD_LEN)
	{
		BUSY_LED_ON;

		uint8_t ram_type		= rx_cmd_buffer[RAM_TYPE_INDEX];
		uint8_t ram_clock_cmd	= rx_cmd_buffer[RAM_CLOCK_CMD_INDEX];

		/* Perform test only if module is connected */
		if(module_is_connected == TRUE)
		{
			/* Check if type is correct */
			if((ram_type == M48T02_RAM_TYPE) || (ram_type == M48T08_RAM_TYPE))
			{
				switch(ram_clock_cmd)
				{
					case START_CLOCK_OSCILLATOR:

						/* Memory size is offset to clock */
						Start_RAM_clock_oscillator((ram_type == M48T02_RAM_TYPE) ? M48T02_RAM_SIZE : M48T08_RAM_SIZE);

						execution_OK = TRUE;

						break;

					case STOP_CLOCK_OSCILLATOR:

						Stop_RAM_clock_oscillator((ram_type == M48T02_RAM_TYPE) ? M48T02_RAM_SIZE : M48T08_RAM_SIZE);

						execution_OK = TRUE;

						break;

					case RESET_CLOCK:

						Reset_RAM_clock((ram_type == M48T02_RAM_TYPE) ? M48T02_RAM_SIZE : M48T08_RAM_SIZE);

						execution_OK = TRUE;

						break;

					default:

						execution_OK = FALSE;

						break;
				}
			}

			module_detection_was_performed = 1;
		}

		/* Compose response */
		tx_response_buffer[CMD_INDEX]				= CONTROL_RAM_CLOCK_COMMAND;
		tx_response_buffer[DETECT_INDEX]			= module_is_connected;
		tx_response_buffer[EXECUTION_OK_INDEX]		= execution_OK;

		/* Send response to PC */
		TX_frame(tx_response_buffer, CONTROL_RAM_CLOCK_CMD_RESPONSE_LEN);

		BUSY_LED_OFF;
	}

	return;
}

void GetRamClock(void)
{
	uint8_t clock_data_valid = TRUE;
	struct time_data clock_data;

	/* Check command length */
	if(rx_cmd_len == GET_RAM_CLOCK_CMD_LEN)
	{
		BUSY_LED_ON;

		uint8_t ram_type = rx_cmd_buffer[RAM_TYPE_INDEX];

		if(module_is_connected == TRUE)
		{
			/* Select correct RAM */
			switch(ram_type)
			{
				case M48T02_RAM_TYPE:

					Get_time_from_RAM_clock(M48T02_RAM_SIZE, &clock_data);

					/* Data OK */
					clock_data_valid = TRUE;

					break;

				case M48T08_RAM_TYPE:

					Get_time_from_RAM_clock(M48T08_RAM_SIZE, &clock_data);

					/* Data OK */
					clock_data_valid = TRUE;

					break;


				default:

					/* Unsupported RAM */
					clock_data_valid = FALSE;

					break;
			}
		}
		else
		{
			/* RAM adapter not connected at all */
			clock_data_valid = FALSE;
		}

		/* Compose response */
		tx_response_buffer[CMD_INDEX] 				= GET_RAM_CLOCK_COMMAND;
		tx_response_buffer[DETECT_INDEX]			= module_is_connected;
		tx_response_buffer[CLOCK_DATA_VALID_INDEX]	= clock_data_valid;

		if(clock_data_valid == TRUE)
		{
			memcpy(&tx_response_buffer[CLOCK_DATA_GET_DATA_INDEX], &clock_data, sizeof(clock_data));
		}
		else
		{
			/* Clear buffer if data is not valid */
			memset(&tx_response_buffer[CLOCK_DATA_GET_DATA_INDEX], 0x00, sizeof(clock_data));
		}

		/* Send response to PC */
		TX_frame(tx_response_buffer, GET_RAM_CLOCK_CMD_RESPONSE_LEN);

		BUSY_LED_OFF;
	}

	return;
}

void SetRamClock(void)
{
	uint8_t execution_OK = TRUE;
	struct time_data clock_data;

	/* Check command length */
	if(rx_cmd_len == SET_RAM_CLOCK_CMD_LEN)
	{
		BUSY_LED_ON;

		uint8_t ram_type		= rx_cmd_buffer[RAM_TYPE_INDEX];

		/* Copy data to clock data structure */
		memcpy(&clock_data, &rx_cmd_buffer[RAM_CLOCK_SET_DATA_INDEX], sizeof(clock_data));

		if(module_is_connected == TRUE)
		{
			/* Select correct RAM */
			switch(ram_type)
			{
				case M48T02_RAM_TYPE:

					Set_time_in_RAM_clock(M48T02_RAM_SIZE, &clock_data);

					/* Executed OK */
					execution_OK = TRUE;

					break;

				case M48T08_RAM_TYPE:

					Set_time_in_RAM_clock(M48T08_RAM_SIZE, &clock_data);

					/* Executed OK */
					execution_OK = TRUE;

					break;


				default:

					/* Unsupported RAM */
					execution_OK = FALSE;

					break;
			}
		}
		else
		{
			/* RAM adapter not connected at all */
			execution_OK = FALSE;
		}

		/* Compose response */
		tx_response_buffer[CMD_INDEX] 			= SET_RAM_CLOCK_COMMAND;
		tx_response_buffer[DETECT_INDEX]		= module_is_connected;
		tx_response_buffer[EXECUTION_OK_INDEX]	= execution_OK;

		/* Send response to PC */
		TX_frame(tx_response_buffer, SET_RAM_CLOCK_CMD_RESPONSE_LEN);

		BUSY_LED_OFF;
	}

	return;
}



inline uint8_t Check_8K_ROM(void)
{
	uint8_t ROM_is_present = ROM_NOT_PRESENT;

	/* Check if ROM is present */
	if(Check_if_ROM_is_present(ROM_SIZE_8K) == TRUE)
	{
		/* ROM is present */

		/* Check if it is empty */
		if(Check_if_ROM_is_empty(ROM_SIZE_8K) == TRUE)
		{
			/* ROM is empty, mark as not present */
			ROM_is_present = ROM_NOT_PRESENT;
		}
		else
		{
			/* Not empty, mark as present */
			ROM_is_present = ROM_IS_PRESENT;
		}
	}
	else
	{
		/* ROM is not present */
		ROM_is_present = ROM_NOT_PRESENT;
	}

	return ROM_is_present;
}

inline uint8_t Check_32K_ROM(void)
{
	uint8_t ROM_is_present = ROM_NOT_PRESENT;

	/* Check if ROM is present */
	if(Check_if_ROM_is_present(ROM_SIZE_32K) == TRUE)
	{
		/* ROM is present */

		/* Check if it is empty */
		if(Check_if_ROM_is_empty(ROM_SIZE_32K) == TRUE)
		{
			/* ROM is empty, mark as not present */
			ROM_is_present = ROM_NOT_PRESENT;
		}
		else
		{
			/* Not empty, mark as present */
			ROM_is_present = ROM_IS_PRESENT;
		}
	}
	else
	{
		/* ROM is not present */
		ROM_is_present = ROM_NOT_PRESENT;
	}

	return ROM_is_present;
}

inline uint8_t Check_64K_ROMs(void)
{
	uint8_t ROM_is_present = ROM_NOT_PRESENT;

	/* Check high bank */
	SET_A15_TO_HIGH;

	if(Check_32K_ROM() == ROM_IS_PRESENT)
	{
		/* High bank is present */

		/* ROM is present */
		ROM_is_present = ROM_IS_PRESENT;
	}
	else
	{
		/* High bank is not present (very rare scenario) */

		/* Check low bank */
		SET_A15_TO_LOW;

		if(Check_32K_ROM() == ROM_IS_PRESENT)
		{
			/* Low bank is present */

			/* So, ROM is present */
			ROM_is_present = ROM_IS_PRESENT;
		}
		else
		{
			/* Neither high nor low bank are present */

			ROM_is_present = ROM_NOT_PRESENT;
		}
	}

	/* Set back to high bank */
	SET_A15_TO_HIGH;

	return ROM_is_present;
}

inline uint8_t Select_correct_bank_for_A_ROM(uint8_t block_number, uint8_t *block_to_read)
{
	uint8_t data_valid = TRUE;

	/* Is this low bank ? */
	if((block_number >= BLOCK_NUMBER_LOW_BANK_MIN_ROM_A) && (block_number <= BLOCK_NUMBER_LOW_BANK_MAX_ROM_A))
	{
		/* A15 is dont care for that ROM */
		SET_A15_TO_HIGH;

		/* Range OK */
		data_valid = TRUE;
	}
	else
	{
		/* Default */
		SET_A15_TO_HIGH;

		/* Wrong range, data invalid */
		data_valid = FALSE;
	}

	/* For ROMA, pass */
	*block_to_read = block_number;

	return data_valid;
}

inline uint8_t Select_correct_bank_for_B_and_C_ROMs(uint8_t block_number, uint8_t *block_to_read)
{
	uint8_t data_valid = TRUE;

	/* Is this low bank ? */
	if((block_number >= BLOCK_NUMBER_LOW_BANK_MIN_ROM_BC) && (block_number <= BLOCK_NUMBER_LOW_BANK_MAX_ROM_BC))
	{
		/* Select low 32K bank */
		SET_A15_TO_LOW;

		/* Low bank */
		*block_to_read = block_number;
	}
	else if((block_number >= BLOCK_NUMBER_HIGH_BANK_MIN_ROM_BC) && (block_number <= BLOCK_NUMBER_HIGH_BANK_MAX_ROM_BC))
	{
		/* Select high 32K bank */
		SET_A15_TO_HIGH;

		/* High, bank, address overlap */
		*block_to_read = block_number - BLOCK_NUMBER_HIGH_BANK_MIN_ROM_BC;
	}
	else
	{
		/* Default */
		SET_A15_TO_HIGH;
		*block_to_read = 0;

		/* Wrong range, data invalid */
		data_valid = FALSE;
	}

	return data_valid;
}

inline void Clear_ROM_info(void)
{
	ROM_A_is_present = ROM_NOT_PRESENT;
	ROM_B_is_present = ROM_NOT_PRESENT;
	ROM_C_is_present = ROM_NOT_PRESENT;

	/* that flag will be set if ROM is connected and detection was performed */
	module_detection_was_performed = FALSE;

	return;
}

ISR(ISR_TIMER_VECTOR)
{
	static uint8_t toggle_led_counter = 0;

	if(IS_ROM_MODULE_PRESENT)
	{
		/* Set connected flag */
		module_is_connected = TRUE;

		/* Set detect LED */
		DETECT_LED_ON;

		/* Clear counter */
		toggle_led_counter = 0;
	}
	else
	{
		/* Reset connected flag */
		module_is_connected = FALSE;

		/* Reset ROM detection flags */
		Clear_ROM_info();

		/* Blink detect LED */
		toggle_led_counter++;
		if(toggle_led_counter >= DETECT_LED_COUNTER_MAX)
		{
			toggle_led_counter = 0;

			DETECT_LED_TOGGLE;
		}
	}

	return;
}
