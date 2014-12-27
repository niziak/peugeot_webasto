/*
 * handler_menu.c
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#include <stdio.h>

#include <avr/io.h>
#include <avr/wdt.h>
#include "types.h"
#include "globals.h"
#include "log.h"
#include "usart0.h"
#include "timer0.h"
#include <events.h>
#include "temperature.h"
#include <nvm.h>


void MENU_vHandleEvent(EVENT_DEF eEvent)
{
    if (eEvent == EV_SHOW_MAIN_MENU) // initialisation event
    {
        eState = ST_MENU_SHOW_MAIN;
    }
    switch (eState)
    {
        case ST_MENU_SHOW_MAIN:
            switch (eEvent)
            {
                case EV_UART_LINE_COMPLETE:
                    DEBUG_MEM(pu8GetLineBuf(), UART_RX_LINE_BUFFER);
                    switch (*pu8GetLineBuf())
                    {
                        case '1':
                            uiHeaterSwitchOffAfterS = 1;
                            LOG_P(PSTR("Heater is ON\n"));
                            break;
                        case '2':
                            uiHeaterSwitchOffAfterS = 0;
                            LOG_P(PSTR("Heater is OFF\n"));
                            break;
                        case '3':
                            LOG_P(PSTR(""\n));
                            TEMP_vCalculateCalibration();
                            TEMP_vReadTemperature();
                            break;
                    }
                    EventPost(EV_SHOW_MAIN_MENU);
                    break;

                case EV_UART_LINE_FULL:
                    EventPost(EV_SHOW_MAIN_MENU);
                    break;

                case EV_SHOW_MAIN_MENU:
                    LOG_NL;
                    LOG_NL;
                    LOG_P(PSTR("1. Turn heater ON\n"));
                    LOG_P(PSTR("2. Turn heater OFF\n"));
                    LOG_P(PSTR("3. Show temperature\n"));
                    LOG_P(PSTR("\n"));
                    LOG_P(PSTR("Choice> "));
                    USART0_vRXWaitForLine();
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
