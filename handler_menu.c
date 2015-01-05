/*
 * handler_menu.c
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "types.h"
#include "globals.h"
#include "log.h"
#include "usart0.h"
#include "timer0.h"
#include <events.h>
#include "temperature.h"
#include <nvm.h>
#include "tools.h"
#include "pulse_det.h"
#include "handler_app.h"
#include "adc.h"
#include "handler_app.h"

int16_t s16GetIntFromString(uint8_t *pu8String)
{
    pu8String++; // skip 'A' from 'A=123'
    if (*pu8String != '=')
    {
        return 0; // TODO error handling
    }
    pu8String++; // skip '=' from 'A=123'
    return atoi((const char*)pu8String); //TODO sth better
}


static void vShowStatus(void)
{
    LOG_NL;
    LOG_P(PSTR("=== STATUS ===\n"));

    ADC_vGetCarVoltage();
    ADC_vPrintCarVoltage();

    //TEMP_vPrintCalibrationData();
    TEMP_vCalculateCalibration();
    TEMP_vReadTemperature();
    TEMP_vPrintTemperature();

        //uint16_t                    auiExpectedPeriodsMS[MAX_PERIODS];
    LOG_P(PSTR("Heater is ..........................%s\n"), WEBASTO_STATE_GET_STR);
    LOG_P(PSTR("Turning light is ...................%s\n"), (bit_is_set(PINB, PINB0)) ? "ON" : "OFF");
    LOG_NL;
    LOG_P(PSTR("=== SETTINGS ===\n"));
    LOG_P(PSTR("A. Sleep when no pulses for ........%d ms\n"),      stSettings.u16IdleWhenNoPulsesMs);
    LOG_P(PSTR("B. Pulse tolerance +/- .............%d ms\n"),      stSettings.u16PulseLenToleranceMs);
    LOG_P(PSTR("C. Heater turn ON time .............%d min\n"),     stSettings.u16HeaterEnabledForMin);
    LOG_P(PSTR("D. Heater max ambient temp .........%d C\n"),       stSettings.s8HeaterEnableMaxTemperature);
    LOG_P(PSTR("E. Voltage divider ratio ...........%d/100\n"),     stSettings.u16VoltageDividerRatio);
    LOG_P(PSTR("F. Voltage with engine .............%d mV\n"),      stSettings.u16VoltageWithEngine);
    LOG_P(PSTR("G. Voltage minimum level ...........%d mV\n"),      stSettings.u16VoltageMinimumLevel);
    LOG_P(PSTR("H. #1 Temp. calibrated point .......%2d C\n"),      pstSettings->astTempCal[0].s8RealTemp);
    LOG_P(PSTR("I. #1 Temp. calibrated point .......%2d RAW\n"),    pstSettings->astTempCal[0].s16ADCTemp);
    LOG_P(PSTR("J. #2 Temp. calibrated point .......%2d C\n"),      pstSettings->astTempCal[1].s8RealTemp);
    LOG_P(PSTR("K. #2 Temp. calibrated point .......%2d RAW\n"),    pstSettings->astTempCal[1].s16ADCTemp);
}

static void vShowMenu(void)
{
    LOG_P(PSTR("=== M E N U ===\n"));
    LOG_P(PSTR("1. Turn heater ON\n"));
    LOG_P(PSTR("2. Turn heater OFF\n"));
    LOG_P(PSTR("3. Show line changes\n"));
    LOG_P(PSTR("4. Pulse pattern - collect now\n"));
    LOG_P(PSTR("5. Pulse pattern - show\n"));
    LOG_P(PSTR("6. Pulse pattern - use collected\n"));
//    LOG_P(PSTR("V. Turn VDIV ON\n"));
//    LOG_P(PSTR("v. Turn VDIV OFF\n"));
    LOG_P(PSTR("S. Save settings\n"));
    LOG_P(PSTR("R. Factory defaults\n"));
    LOG_P(PSTR("p. Never ended sleep (power down)\n"));
    LOG_P(PSTR("q. Quit (reboot)\n"));
    LOG_P(PSTR("ENTER. refresh\n"));
}

#define MAX_COLUMNS 80
static uint8_t u8Column;

/**
 * Set state of menu handler to @ref ST_MENU_SHOW_MAIN and generte event @ref EV_SHOW_MAIN_MENU
 */
void MENU_vShowMainMenu(void)
{
    eHandler = HND_MENU;
    eState = ST_MENU_SHOW_MAIN;
    EventPost(EV_SHOW_MAIN_MENU);
}


void MENU_vHandleEvent(EVENT_DEF eEvent)
{
    switch (eState)
    {
        default:
            RESET_P(PSTR("Unhandled state!"));
            break;

        case ST_MENU_WAIT_FOR_PATTERN:
            switch (eEvent)
            {
                case EV_CHECK_PATTERN: // send from Timer1
                case EV_PULSE_TOO_LONG: // From T1 OVF or T0 Idle time reached
                    LOG_P(PSTR("Collected!\n"));
                    MENU_vShowMainMenu();
                    break;

                default:
                case EV_CLOCK_1S:
                    wdt_reset();
                    break;
            }
            break;

        case ST_MENU_SHOW_MAIN:
            switch (eEvent)
            {
                case EV_UART_LINE_COMPLETE:
//                    DEBUG_MEM(pu8GetLineBuf(), UART_RX_LINE_BUFFER);
                    switch (*pu8GetLineBuf())
                    {
                        case 'A':
                            stSettings.u16IdleWhenNoPulsesMs = (uint16_t)s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'B':
                            stSettings.u16PulseLenToleranceMs = (uint16_t)s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'C':
                            stSettings.u16HeaterEnabledForMin =  (uint16_t)s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'D':
                            stSettings.s8HeaterEnableMaxTemperature = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'E':
                            stSettings.u16VoltageDividerRatio = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'F':
                            stSettings.u16VoltageWithEngine = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'G':
                            stSettings.u16VoltageMinimumLevel = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'H':
                            pstSettings->astTempCal[0].s8RealTemp = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'I':
                            pstSettings->astTempCal[0].s16ADCTemp = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'J':
                            pstSettings->astTempCal[1].s8RealTemp = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case 'K':
                            pstSettings->astTempCal[1].s16ADCTemp = s16GetIntFromString(pu8GetLineBuf());
                            break;

                        case '1':
                            uiHeaterSwitchOffAfterS = 1;
                            LOG_P(PSTR("Heater is ON\n"));
                            break;

                        case '2':
                            uiHeaterSwitchOffAfterS = 0;
                            LOG_P(PSTR("Heater is OFF\n"));
                            break;

                        case '3':
                            APP_vEnablePinChangeEvents();
                            u8Column = 0;
                            break;

                        case '4':
                            eState = ST_MENU_WAIT_FOR_PATTERN;
                            PIN_CHANGE_INT_ENABLE
                            PD_vWaitForNextSeries();
                            break;

                        case '5':
                            PD_bAnalyzeCollectedPulses(FALSE);
                            break;

                        case '6':
                            {
                                uint16_t uiCollectedIndex;
                                for (uiCollectedIndex=0; uiCollectedIndex < MAX_PERIODS; uiCollectedIndex++)
                                {
                                    uint32_t uiPeriodMS = (uint32_t)auiPeriods[uiCollectedIndex] * (uint32_t)TIMER1_TICK_US / (uint32_t)1000;
                                    pstSettings->auiExpectedPeriodsMS[uiCollectedIndex] = uiPeriodMS;
                                }
                            }
                            break;

//                        case 'V':
//                            VOLTAGE_DIVIDER_ENABLE;
//                            break;
//
//                        case 'v':
//                            VOLTAGE_DIVIDER_DISABLE;
//                            break;

                        case 'R':
                            NVM_vRestoreFactory();
                            WdtResetHW();
                            break;

                        case 'S':
                            NVM_vSaveSettings();
                            break;

                        case 'p':
                            //USART0_vRXWaitForLine();
                            set_sleep_mode (SLEEP_MODE_PWR_DOWN);
                            WdtDisable();
                            cli();
                            sleep_mode(); // <--- POWER DOWN
                            break;

                        case 'q':
                            WdtResetHW();
                            break;
                    }
                    EventPost(EV_SHOW_MAIN_MENU);
                    break;

                case EV_UART_LINE_FULL:
                    EventPost(EV_SHOW_MAIN_MENU);
                    break;

                case EV_SHOW_MAIN_MENU:
                    LOG_NL;
                    vShowStatus();
                    LOG_NL;
                    vShowMenu();
                    LOG_P(PSTR("\n"));
                    LOG_P(PSTR("Choice> "));
                    USART0_vRXWaitForLine();
                    break;

                case EV_PIN_CHANGED_L:
                    LOG_P(PSTR("_"));
                    u8Column++;
                    if (u8Column == MAX_COLUMNS)
                    {
                        LOG_NL;
                        u8Column = 0;
                    }
                    break;

                case EV_PIN_CHANGED_H:
                    LOG_P(PSTR("#"));
                    u8Column++;
                    if (u8Column == MAX_COLUMNS)
                    {
                        LOG_NL;
                        u8Column = 0;
                    }
                    break;

                case EV_CLOCK_1S:
                    wdt_reset();
                    break;

                default:
                    break;
            }
            break;
    }
}

void MENU_vExecuteMenu(void)
{
    TEMP_vReadCalibrationDataFromConsole();

    NVM_vSaveSettings();
}

static BOOL bWaitForCR (void)
{
    vSetUserTimeout (1000);
    while (! bIsTimedOut() )
    {
        wdt_reset();
        if ((USART0_bIsByteAvail()) && (USART0_ucGetByte() == '\r'))
        {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL MENU_bCheckEnterSequence(void)
{
    uint8_t ucI;
    BOOL bFirstCR = FALSE;

    LOG_P(PSTR("\n\nPress ENTER twice for menu... "));

    for (ucI=MENU_COUNTDOWN; ucI>0; ucI--)
    {
        LOG_P(PSTR("%d "), ucI);
        if (bWaitForCR())
        {
            if (bFirstCR)
            {
                LOG_P(PSTR("\nEntering menu...\n"));
                return TRUE;
            }
            bFirstCR = TRUE;
        }
    }
    LOG_NL;
    return FALSE;
}
