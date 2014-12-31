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
#include "adc.h"

#define PIN_CHANGE_INT_ENABLE       { PCICR  |=   _BV(PCIE0); PCMSK0 |=   _BV(PCINT0); }
#define PIN_CHANGE_INT_DISABLE      { PCICR  &= ~ _BV(PCIE0); PCMSK0 &= ~ _BV(PCINT0); }

static volatile BOOL bSendPinChangeEvents = FALSE;
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
    if (bSendPinChangeEvents)
    {
        if (bit_is_set(PINB, PINB0))
        {
            EventPostFromIRQ(EV_PIN_CHANGED_H);
        }
        else
        {
            EventPostFromIRQ(EV_PIN_CHANGED_L);
        }
    }
}

void APP_vEnablePinChangeEvents(void)
{
    PIN_CHANGE_INT_ENABLE;
    bSendPinChangeEvents = TRUE;
}

static void vStopWebasto(void)
{
    LOG_P(PSTR("\n\n!!! Stopping heater!\n\n"));
    uiHeaterSwitchOffAfterS = 0;
    EventPost(EV_WAIT_FOR_PULSES);
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

            case EV_CHECK_PATTERN: // send from Timer1
                PIN_CHANGE_INT_DISABLE
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
                LOG_P(PSTR("\n\n!!! Pattern match.\n\n"));
                EventPost(EV_READ_TEMPERATURE);
                break;

            case EV_READ_TEMPERATURE:
                LOG_P(PSTR("Reading ambient temperature...\n"));
                TEMP_vReadTemperature();

                if (s16Temperature > pstSettings->s8HeaterEnableMaxTemperature)
                {
                        LOG_P(PSTR("Ambient temp %d > %d set. Nothing to do.\n"), s16Temperature, pstSettings->s8HeaterEnableMaxTemperature);
                        EventPost(EV_WAIT_FOR_PULSES);
                }
                else
                {
                        LOG_P(PSTR("Ambient temp %d <= %d set.\n"), s16Temperature, pstSettings->s8HeaterEnableMaxTemperature);
                        EventPost(EV_START_WEBASTO);
                }
                break;

            case EV_STOP_WEBASTO:
                vStopWebasto();
                break;

            case EV_START_WEBASTO:
                LOG_P(PSTR("\n\n!!! Starting heater!\n\n"));
                uiHeaterSwitchOffAfterS = pstSettings->u16HeaterEnabledForMin * 60;
                //no break;
            case EV_CLOCK_1S:
                wdt_reset();
                if (uiHeaterSwitchOffAfterS>0)
                {
                    ADC_vGetCarVoltage();
                    // check if voltage drops below critical level
                    if (u16CarVoltage < stSettings.u16VoltageMinimumLevel)
                    {
                        LOG_P(PSTR("\n\n!!! Voltage too low!\n\n"));
                        vStopWebasto();
                        break;
                    }

                    if (u16CarVoltage > stSettings.u16VoltageWithEngine)
                    {
                        LOG_P(PSTR("\n\n!!! Engine running!\n\n"));
                        vStopWebasto();
                        break;
                    }

                    uiHeaterSwitchOffAfterS--;
                    LOG_P(PSTR("\tHeater enabled for %d sec.\n"), uiHeaterSwitchOffAfterS);
                    if (uiHeaterSwitchOffAfterS==0)
                    {
                        vStopWebasto();
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



