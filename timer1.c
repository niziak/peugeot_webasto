/*
 * timer1.c
 *
 *  Created on: Dec 18, 2014
 *      Author: niziak
 */

/**
 *         +----+  +----+
 *         |    |  |    |
 * --------+    +--+    +------
 *
 *         |    |  |
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
#include "types.h"
#include "events.h"


static volatile BOOL bRisingEdge;

volatile uint16_t T1Ovs1, T1Ovs2;
volatile uint16_t Capt1, Capt2, Capt3;

#define T1_IC_SET_RISING  { TCCR1B |=   (1<<ICES1); }
#define T1_IC_SET_FALLING { TCCR1B &= ~ (1<<ICES1); }

ISR(TIMER1_OVF_vect)
{
	EventPostFromIRQ(SYS_T1INT);
	T1Ovs1++;
}


ISR(TIMER1_CAPT_vect)
{
	if (bRisingEdge)
	{
		Capt1 = ICR1;
		T1_IC_SET_FALLING
		bRisingEdge = FALSE;
	}
	else
	{
		Capt2 = ICR1;
		T1_IC_SET_RISING
		bRisingEdge = TRUE;
	}
}


void TIMER1_vInit(void)
{
	TCCR1B |= (   (1<<CS12) | (1<<CS10) /* Prescaler Fclk/1024 */
			    | (1<<ICNC1)            /* IC noise canceller */
			  );

   	bRisingEdge = TRUE;
   	T1_IC_SET_RISING
   	TCNT1=0; // init value for T1

   	TIMSK1 |= (1<<ICIE1) | (1<<TOIE1); // Input Capture and Overflow interrupts enable
}

void TIMER1_vStop(void)
{
	TIMSK1 &= ~ ((1<<ICIE1)|(1<<TOIE1));
}
