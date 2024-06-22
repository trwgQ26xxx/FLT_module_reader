
#ifndef _SERIAL_TRANSMISSION_H
#define _SERIAL_TRANSMISSION_H

#include <stdint.h>

void TX_text(char *text);

void Reset_RX_Transmission(void);

uint8_t Process_RX_Transmission(uint8_t *buffer, uint8_t *rx_cmd_len);

void TX_frame(uint8_t *buffer, uint16_t len);

#endif

