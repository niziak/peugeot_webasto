/*
 * pulse_det.c
 *
 *  Created on: 21 Dec 2014
 *      Author: niziak
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdio.h>

#include "config.h"
#include "timer0.h"
#include "timer1.h"
#include "log.h"
#include "globals.h"


const uint16_t auiExpectedPeriodsMS[MAX_PERIODS] =
{
        /*  1 */     200, 100,
        /*  2 */     200, 100,
        /*  3 */     200, 100,
        /*  4 */     200, 100,
        /*  5 */     200, 100,
        /*  6 */     200, 100,
        /*  7 */     200, 100,
        /*  8 */     200, 100,
        /*  9 */     200, 100,
        /* 10 */     200,
};


void vWaitForNextSeries(void)
{
    TIMER1_vStop();
    LOG_P(PSTR("Waiting for external event (power down)...\n"));
    for (uint8_t i=0; i<10; i++)
    {
        LOG_P(PSTR(".\n"));
    }
    _delay_ms(20); //give UART chance to transmit

    TIMER1_vInit(); // start pulse measure
    sei();
    // Go into very deep sleep, waiting only for external interrupt on PCINT0 (PB0)
    set_sleep_mode (SLEEP_MODE_PWR_DOWN); // wait for int (1ms timer)
    wdt_disable();

    sleep_mode(); // <--- POWER DOWN

    wdt_enable(WDTO_2S);
    wdt_reset();
    cli();

    ulSystemTickMS = 0;

    TIMER0_vInit();
    sei();

    LOG_P(PSTR("Line change - back from power down mode!\n"));
}


BOOL bAnalyzeCollectedPulses(void)
{

        uint8_t uiCollectedIndex, uiExpectedIndex;
        uiExpectedIndex = 0;
        BOOL bFirstMatched = FALSE;
        BOOL bOK = FALSE;

        LOG_P(PSTR("Analyzing pulses...\n"));
        for (uiCollectedIndex=0; uiCollectedIndex < MAX_PERIODS; uiCollectedIndex++)
        {
            uint32_t uiPeriodMS = (uint32_t)auiPeriods[uiCollectedIndex] * (uint32_t)TIMER1_TICK_US / (uint32_t)1000;
            LOG_P(PSTR("#%03d got %5u ms"), uiCollectedIndex, uiPeriodMS);
            LOG_P(PSTR("    wait for   "));
            LOG_P(PSTR("%5u ms #%03d "), auiExpectedPeriodsMS[uiExpectedIndex], uiExpectedIndex);

            if (    (auiExpectedPeriodsMS[uiExpectedIndex] + PULSE_LEN_TOLERANCE_MS > uiPeriodMS)
                 && (auiExpectedPeriodsMS[uiExpectedIndex] - PULSE_LEN_TOLERANCE_MS < uiPeriodMS) )
            {
                bFirstMatched = TRUE;
                bOK = TRUE;
                LOG_P(PSTR("ok\n"));
            }
            else
            // value not matched
            {
                if (bFirstMatched)
                {
                    // fail - sequence started, but value is not matched
                    // if failed was not last (last entry = 0)
                    if (auiExpectedPeriodsMS[uiExpectedIndex] >0)
                    {
                        bOK = FALSE;
                        LOG_P(PSTR("NOK"));
                    }
                    LOG_vNL();
                }
                else
                {
                    LOG_P(PSTR("\n"));
                }
            }

            if (bFirstMatched)
            {
                uiExpectedIndex++;
            }

//            if (bFailed)
//            {
//                break;
//            }
        }
        return bOK;
}
