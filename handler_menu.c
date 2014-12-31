/*
 * handler_menu.c
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#include <stdio.h>
#include <stdlib.h>

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


void TEMP_vReadCalibrationDataFromConsole(void)
{
    TEMP_vPrintCalibrationData();
    int status;

    for (uint8_t i=0; i<TEMP_CALIB_POINTS; i++)
    {
        LOG_NL;
        do
        {
            LOG_P(PSTR("\t#%d new RAW="),i );
            status = scanf("%d", &pstSettings->astTempCal[i].s16ADCTemp);
            LOG_P(PSTR("Scanf returns %d value is %d\n"), status, pstSettings->astTempCal[i].s16ADCTemp);
            if (status !=1 )
            {
                LOG_P(PSTR("consumed char=0x%02X\n"), getchar());
            }
        } while (status != 1);
        _delay_ms(5000);
        LOG_P(PSTR("\t#%d new   T="),i ); status = scanf("%d", &pstSettings->astTempCal[i].s8RealTemp);
        LOG_P(PSTR("Scanf returns %d value is %d\n"), status, pstSettings->astTempCal[i].s8RealTemp);
    }
    LOG_NL;
}

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
    TEMP_vCalculateCalibration();
    TEMP_vReadTemperature();
    ADC_vGetCarVoltage();

        //uint16_t                    auiExpectedPeriodsMS[MAX_PERIODS];
    LOG_P(PSTR("Heater is ..........................%s\n"), WEBASTO_STATE_GET_STR);
    LOG_NL;
    LOG_P(PSTR("=== SETTINGS ===\n"));
    LOG_P(PSTR("A. Sleep when no pulses for ........%d ms\n"),  stSettings.u16IdleWhenNoPulsesMs);
    LOG_P(PSTR("B. Pulse tolerance +/- .............%d ms\n"),  stSettings.u16PulseLenToleranceMs);
    LOG_P(PSTR("C. Heater turn ON time .............%d min\n"), stSettings.u16HeaterEnabledForMin);
    LOG_P(PSTR("D. Heater max ambient temp .........%d C\n"),   stSettings.s8HeaterEnableMaxTemperature);
    LOG_P(PSTR("E. Voltage divider ratio ...........%d/100\n"), stSettings.u16VoltageDividerRatio);
    LOG_P(PSTR("F. Voltage with engine .............%d mV\n"),  stSettings.u16VoltageWithEngine);
    LOG_P(PSTR("G. Voltage minimum level ...........%d mV\n"),  stSettings.u16VoltageMinimumLevel);
}

static void vShowMenu(void)
{
    LOG_P(PSTR("=== M E N U ===\n"));
    LOG_P(PSTR("1. Turn heater ON\n"));
    LOG_P(PSTR("2. Turn heater OFF\n"));
    LOG_P(PSTR("3. New temperature calibration values\n"));
    LOG_P(PSTR("4. New pulse pattern\n"));
    LOG_P(PSTR("5. Show pulses\n"));
//    LOG_P(PSTR("V. Turn VDIV ON\n"));
//    LOG_P(PSTR("v. Turn VDIV OFF\n"));
    LOG_P(PSTR("S. Save settings\n"));
    LOG_P(PSTR("R. Factory defaults\n"));
    LOG_P(PSTR("p. Never ended sleep (power down)\n"));
    LOG_P(PSTR("q. Quit (reboot)\n"));
    LOG_P(PSTR("ENTER. refresh\n"));
}



void MENU_vHandleEvent(EVENT_DEF eEvent)
{
    if (eEvent == EV_SHOW_MAIN_MENU) // Initialisation event
    {
        eState = ST_MENU_SHOW_MAIN;
    }
    switch (eState)
    {
        default:
            RESET_P(PSTR("Unhandled state!"));
            break;

        case ST_MENU_SHOW_MAIN:
            switch (eEvent)
            {
                case EV_UART_LINE_COMPLETE:
                    DEBUG_MEM(pu8GetLineBuf(), UART_RX_LINE_BUFFER);
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

                        case '1':
                            uiHeaterSwitchOffAfterS = 1;
                            LOG_P(PSTR("Heater is ON\n"));
                            break;

                        case '2':
                            uiHeaterSwitchOffAfterS = 0;
                            LOG_P(PSTR("Heater is OFF\n"));
                            break;

                        case '3':
                            LOG_P(PSTR("\n"));
                            TEMP_vCalculateCalibration();
                            TEMP_vReadTemperature();
                            break;

                        case '4':
                            vWaitForNextSeries();
                            break;

                        case '5':
                            APP_vEnablePinChangeEvents();
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
                    LOG_P(PSTR(" 0\n"));
                    break;

                case EV_PIN_CHANGED_H:
                    LOG_P(PSTR(" 1\n"));
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
