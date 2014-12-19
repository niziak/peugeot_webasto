/*
 * timer1.c
 *
 *  Created on: Dec 18, 2014
 *      Author: niziak
 */

/**
 *
 *           +----+  +----+
 *           |    |  |    |
 * ----------+    +--+    +--------
 *
 *           |    |  |
 *
 *
 *
 *
 * 16MHz / 1024  can count up to 4.19 seconds,
 * 1000 Timer1 ticks=0,064 s = 64ms
 *  100        ticks         = 6,4ms
 *   10        ticks         = 0,64ms
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "types.h"
#include "events.h"
#include "timer1.h"
#include "config.h"
#include <string.h>

static volatile BOOL bRisingEdge;


volatile uint16_t auiPeriods[MAX_PERIODS];
volatile uint8_t  ucWriteIndex;

#define T1_IC_SET_RISING  { TCCR1B |=   (1<<ICES1); }
#define T1_IC_SET_FALLING { TCCR1B &= ~ (1<<ICES1); }

ISR(TIMER1_OVF_vect)
{
//	EventPostFromIRQ(SYS_T1INT);
//	T1Ovs1++;
}


ISR(TIMER1_CAPT_vect)
{
    uint16_t uiCRTemp;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        uiCRTemp = ICR1;
        TCNT1 = 0; // start counting from 0
    }
    if (ucWriteIndex == MAX_PERIODS)
    {
        ucWriteIndex = 0;
        TIMER1_vStop();
        return;
    }
    auiPeriods[ucWriteIndex++] = uiCRTemp;

	if (bRisingEdge)
	{
		T1_IC_SET_FALLING
		bRisingEdge = FALSE;
	}
	else
	{
		T1_IC_SET_RISING
		bRisingEdge = TRUE;
	}
}


void TIMER1_vInit(void)
{
    TIMER1_vStop();

    ucWriteIndex = 0;
    memset((uint16_t*)&(auiPeriods[0]), 0, sizeof(auiPeriods));

   	bRisingEdge = TRUE;
   	T1_IC_SET_RISING
   	TCNT1=0; // init value for T1

   	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1); /* IC INT enable + Timer1 OVF INT enable */

    TCCR1B |= (   (1<<CS12) | (1<<CS10) /* Prescaler Fclk/1024 = START TIMER*/
                | (1<<ICNC1)            /* IC noise canceller */
              );
}

void TIMER1_vStop(void)
{
    TCCR1B = 0; /* stop Timer1 */
	TIMSK1 &= ~ ((1<<ICIE1)|(1<<TOIE1)); /* IC INT disable + Timer1 OVF INT disable */
}
