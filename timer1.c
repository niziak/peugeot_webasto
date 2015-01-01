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
#include <avr/pgmspace.h>
#include <util/atomic.h>
#include "types.h"
#include "events.h"
#include "timer1.h"
#include "config.h"
#include "log.h"
#include <string.h>
#include "pulse_det.h"

static volatile BOOL bRisingEdge;



#define T1_IC_SET_RISING  { TCCR1B |=   (1<<ICES1); }
#define T1_IC_SET_FALLING { TCCR1B &= ~ (1<<ICES1); }

ISR(TIMER1_OVF_vect)
{
    TIMER1_vStop();
	EventPostFromIRQ(EV_PULSE_TOO_LONG);
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
        //TODO TIMER1_vStop(); // not work from interrupt
        EventPostFromIRQ(EV_CHECK_PATTERN);
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
    //DEBUG_P(PSTR("---TIMER1_vInit\n"));
    TIMER1_vStop();

    PD_vClearStoredPattern();

   	bRisingEdge = TRUE;
   	T1_IC_SET_RISING
   	TCNT1=0; // init value for T1

   	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1); /* IC INT enable + Timer1 OVF INT enable */

    TCCR1B |= (   (1<<CS12) | (1<<CS10) /* Prescaler Fclk/1024 = START TIMER*/
                | (1<<ICNC1)            /* IC noise canceller: The filter function requires four successive equal valued
                                           samples of the ICP1 pin for changing its output */
              );
}

void TIMER1_vStop(void)
{
    //DEBUG_P(PSTR("---TIMER1_vStop\n"));
	TIMSK1 &= ~ ( _BV(ICIE1) | _BV(TOIE1) ); /* IC INT disable + Timer1 OVF INT disable */
    TCCR1B = 0; /* stop Timer1 */
}
