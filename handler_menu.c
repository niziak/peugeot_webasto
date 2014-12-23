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
