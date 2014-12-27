/*
 * timer0.c
 *
 *  Created on: Sep 11, 2013
 *      Author: nizinski_w
 */

#include <avr/interrupt.h>
#include <avr/io.h>

#include <config.h>
#include "globals.h"
#include <events.h>
#include "log.h"
#include "timer0.h"
#include "tools.h"


#define WITH_INT_OVERLAP_DETECTION      TRUE

/* ------------------------------------------- */
static volatile uint8_t       u8UITimeoutS;     ///< when @ref bInMenu, this is a GUI timeout timer
void vResetUITimeout (void)
{
    u8UITimeoutS = UI_TO_S;
}

/* ------------------------------------------- */
static volatile uint32_t u32UserTimeout;

void vSetUserTimeout (uint32_t u32TimeoutMS)
{
    u32UserTimeout = u32TimeoutMS;
}

BOOL bIsTimedOut (void)
{
    return (u32UserTimeout == 0);
}

/* ------------------------------------------- */

#if (WITH_INT_OVERLAP_DETECTION)
static volatile BOOL bInISR = FALSE;
#endif

/**
 *
 * @param TIMER0_OVF_vect
 * @param ISR_NOBLOCK
 */
#if (WITH_INT_OVERLAP_DETECTION)
ISR(TIMER0_OVF_vect, ISR_NOBLOCK)
#else
ISR(TIMER0_OVF_vect)
#endif
{
    RESET_TIMER0_CNT;
#if (WITH_INT_OVERLAP_DETECTION)
    if (bInISR==TRUE)
    {
        RESET("TIM OVLP");
    }
    bInISR = TRUE;
#endif
    ulSystemTickMS ++;



    if (u32UserTimeout>0)
    {
        u32UserTimeout--;
    }

    if (IDLE_TIME_DISABLED != uiIdleTimeMS)
    {
        uiIdleTimeMS++;
    }

    EventTimerTickEveryMS();
    if (pstSettings->u16IdleWhenNoPulsesMs == uiIdleTimeMS)
    {
        EventPostFromIRQ(EV_PULSE_TOO_LONG);
    }

    // ONE SECOND TICK
    if ((ulSystemTickMS % 1000) == 0)
    {
        EventPostFromIRQ(EV_CLOCK_1S); // no event handler for menu mode
        ulSystemTickS++;
        if (u8UITimeoutS > 0)
        {
            if (--u8UITimeoutS == 0)
            {
                WdtResetHW();
            }
        }

    }

#if (WITH_INT_OVERLAP_DETECTION)
    bInISR = FALSE;
#endif
}

/**
 * Initialize system timer
 */
void TIMER0_vInit(void)
{
#if defined (__AVR_ATmega8__)

    TIMSK |= (1<<TOIE0);    // enable timer0 overflow int
    RESET_TIMER0_CNT;
    TCCR0 = (1<<CS00) | (1<<CS02);      // start timer with /1024 prescaler 8000000/1024 = 7812 /s = timer tick every 128us * 256  = 32ms

#elif defined (__AVR_ATmega328P__)

    TIMSK0 |= _BV(TOIE0);    // enable timer0 overflow int
    RESET_TIMER0_CNT;
    //TCCR0B = _BV(CS00) | _BV(CS02);      // start timer with /1024 prescaler 16000000/1024 = 15625 /s = timer tick every 64us * 256  = 16,384ms
    TCCR0B = _BV(CS00) | _BV(CS01);      // start timer with /64 prescaler 16000000/64 = 250000 /s = timer tick every 4us * 256  = 1024usms
    //TCCR0B =  _BV(CS02);      // start timer with /256 prescaler 16000000/256

#else

    #error "CPU!"
#endif

}
