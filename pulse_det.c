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
#include <string.h>

#include "config.h"
#include "timer0.h"
#include "timer1.h"
#include "log.h"
#include "globals.h"
#include "usart0.h"
#include "tools.h"

uint16_t auiPeriods[MAX_PERIODS];
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

#define BAR_GRAPH_WIDTH 20
#define BAR_GRAPH_MAX_VALUE (stSettings.u16IdleWhenNoPulsesMs)

static void vShowBarGraph (uint16_t u16Value)
{
    // show graph
    LOG_P(PSTR("["));
    for (uint8_t i=0; i<BAR_GRAPH_WIDTH; i++)
    {
        if ( (i * BAR_GRAPH_MAX_VALUE / BAR_GRAPH_WIDTH) < u16Value)
        {
            LOG_P(PSTR("#"));
        }
        else
        {
            LOG_P(PSTR(" "));
        }
    }
    LOG_P(PSTR("]"));
}

BOOL PD_bAnalyzeCollectedPulses(BOOL bCompare)
{

        uint8_t uiCollectedIndex, uiExpectedIndex;
        uiExpectedIndex = 0;
        BOOL bFirstMatched = FALSE;
        BOOL bOK = FALSE;

        // skip zero length pulses in expected pattern
        while (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex++] == 0);

        LOG_P(PSTR("Analyzing pulses...\n"));
        for (uiCollectedIndex=0; uiCollectedIndex < MAX_PERIODS; uiCollectedIndex++)
        {
            uint32_t uiPeriodMS = (uint32_t)auiPeriods[uiCollectedIndex] * (uint32_t)TIMER1_TICK_US / (uint32_t)1000;
            LOG_P(PSTR("got [%03d] %5u ms"), uiCollectedIndex, uiPeriodMS);
            LOG_P(PSTR("  wants  "));
            LOG_P(PSTR("[%03d] %5u ms "), uiExpectedIndex, pstSettings->auiExpectedPeriodsMS[uiExpectedIndex]);

            if (bCompare)
            {
                if (    (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex] + pstSettings->u16PulseLenToleranceMs > uiPeriodMS)
                     && (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex] - pstSettings->u16PulseLenToleranceMs < uiPeriodMS) )
                {
                    bFirstMatched = TRUE;
                    bOK = TRUE;
                    LOG_P(PSTR("ok "));
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
                    }
                    else
                    {
                        LOG_P(PSTR("   "));
                    }
                }

                if (bFirstMatched)
                {
                    uiExpectedIndex++;
                }

            } // if bCompare

            if (bCompare == FALSE)
            {
                uiExpectedIndex++;
            }

            vShowBarGraph (uiPeriodMS);
            LOG_P(PSTR(" "));
            vShowBarGraph (pstSettings->auiExpectedPeriodsMS[uiExpectedIndex-1]);

            LOG_NL;

//            if (bFailed)
//            {
//                break;
//            }
        }
        return bOK;
}
