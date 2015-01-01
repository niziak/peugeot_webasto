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
#include <util/atomic.h>

#include <stdio.h>

#include "config.h"
#include "timer0.h"
#include "timer1.h"
#include "log.h"
#include "globals.h"
#include "usart0.h"
#include "tools.h"

volatile uint16_t auiPeriods[MAX_PERIODS];
volatile uint8_t  ucWriteIndex;

/**
 * Write zeros to pattern table
 */
void PD_vClearStoredPattern(void)
{
    ucWriteIndex = 0;
    memset((uint16_t*)&(auiPeriods[0]), 0, sizeof(auiPeriods));
}

/**
 * Disable watchdog, and go to power down mode (only external INT can wake-up)
 *
 */
void PD_vWaitForNextSeries(void)
{
    //USART0_vRXWaitForLine(); // RX led is on without this
    TIMER1_vStop();
    LOG_P(PSTR("Waiting for external event (power down)...\n"));
    for (uint8_t i=0; i<10; i++)
    {
        LOG_P(PSTR(".\n"));
    }
    USART0_vFlush();

    TIMER1_vInit(); // start pulse measure
    sei();
    // Go into very deep sleep, waiting only for external interrupt on PCINT0 (PB0)
    set_sleep_mode (SLEEP_MODE_PWR_DOWN); // wait for int (1ms timer)
    WdtDisable();

    sleep_mode(); // <--- POWER DOWN

    wdt_reset();
    WdtEnable();

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        ulSystemTickMS = 0;
        TIMER0_vInit();
    }

    LOG_P(PSTR("Line change - back from power down mode!\n"));
    //USART0_RXDisable();
}


BOOL PD_bAnalyzeCollectedPulses(void)
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
            LOG_P(PSTR("%5u ms #%03d "), pstSettings->auiExpectedPeriodsMS[uiExpectedIndex], uiExpectedIndex);

            if (    (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex] + pstSettings->u16PulseLenToleranceMs > uiPeriodMS)
                 && (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex] - pstSettings->u16PulseLenToleranceMs < uiPeriodMS) )
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
                    if (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex] >0)
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
