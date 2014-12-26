/*
 * handler_app.c
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <events.h>
#include "globals.h"
#include "types.h"
#include "log.h"
#include "temperature.h"
#include "pulse_det.h"
#include <stdio.h>
#include "usart0.h"

#define PIN_CHANGE_INT_ENABLE       { PCICR  |=   _BV(PCIE0); PCMSK0 |=   _BV(PCINT0); }
#define PIN_CHANGE_INT_DISABLE      { PCICR  &= ~ _BV(PCIE0); PCMSK0 &= ~ _BV(PCINT0); }

/**
 * ISR for PCINT0
 * PCINT[7:0] are serviced by PCINT0_vect
 */
ISR(PCINT0_vect)
{
    if (bit_is_set(PINB, PINB0))
    {
        uiIdleTimeMS = 0;
    }
}

void APP_vHandleEvent(EVENT_DEF eEvent)
{
    switch (eEvent)
        {
            case EV_WAIT_FOR_PULSES:
                uiHeaterSwitchOffAfterS = 0;
                PIN_CHANGE_INT_ENABLE /// from now @ref uiIdleTimeMS is used for pulse detector
                vWaitForNextSeries();
                break;

            case EV_CHECK_PATTERN:
                if (bAnalyzeCollectedPulses())
                {
                    EventPost(EV_GOOD_PATTERN);
                }
                else
                {
                    EventPost(EV_WRONG_PATTERN);
                }
                break;

            case EV_GOOD_PATTERN:
                LOG_P(PSTR("Pattern match.\n"));
                EventPost(EV_READ_TEMPERATURE);
                break;

            case EV_READ_TEMPERATURE:
                LOG_P(PSTR("Reading ambient temperature...\n"));
                TEMP_vReadTemperature();
                LOG_P(PSTR("Temp=%d\n"), iTemp);

                if (iTemp > pstSettings->u8HeaterEnableMaxTemperature)
                {
                        LOG_P(PSTR("Ambient temp %d > %d set. Nothing to do.\n"), iTemp, pstSettings->u8HeaterEnableMaxTemperature);
                        EventPost(EV_WAIT_FOR_PULSES);
                }
                else
                {
                        LOG_P(PSTR("Ambient temp %d <= %d set.\n"),iTemp, pstSettings->u8HeaterEnableMaxTemperature);
                        EventPost(EV_START_WEBASTO);
                }
                break;

            case EV_START_WEBASTO:
                LOG_P(PSTR("Starting heater!\n"));
                uiHeaterSwitchOffAfterS = pstSettings->u16HeaterEnabledForMin * 60;
                break;

            case EV_CLOCK_1S:
                wdt_reset();
                if (uiHeaterSwitchOffAfterS>0)
                {
                    uiHeaterSwitchOffAfterS--;
                    LOG_P(PSTR("\tHeater enabled for %d sec.\n"), uiHeaterSwitchOffAfterS);
                    if (uiHeaterSwitchOffAfterS==0)
                    {
                     LOG_P(PSTR("\tStopping heater!\n"), uiHeaterSwitchOffAfterS);
                        EventPost(EV_WAIT_FOR_PULSES);
                    }
                }
                break;

            case EV_PULSE_TOO_LONG:
                LOG_P(PSTR("Last pulse too long.\n"));
                EventPost(EV_CHECK_PATTERN);
                break;

            case EV_WRONG_PATTERN:
                LOG_P(PSTR("Bad pattern!\n"));
                EventPost(EV_WAIT_FOR_PULSES);
                break;

            case EV_UART_LINE_COMPLETE:
            case EV_UART_LINE_FULL:
                DEBUG_MEM(pu8GetLineBuf(), UART_RX_LINE_BUFFER);
                break;
            default:
                RESET_P(PSTR("unh event!"));
                break;
        } //switch (eEvent)
}



