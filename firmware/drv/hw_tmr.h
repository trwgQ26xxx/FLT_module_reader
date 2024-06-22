

#ifndef _DRV_HW_TMR_H
#define _DRV_HW_TMR_H

#include <stdint.h>

#include <avr/interrupt.h>

#define ISR_TIMER_VECTOR	TIMER1_COMPA_vect

void Init_TMR(uint16_t time);

#endif
