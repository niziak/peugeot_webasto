/*
 * handler_startup.c
 *
 *  Created on: 27 Dec 2014
 *      Author: niziak
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

void STARTUP_vHandleEvent(EVENT_DEF eEvent)
{
           switch (eState)
           {
               default:
                   RESET_P("Unknown state!");
                   break;

               case ST_CHECK_FOR_MENU_ENTER:
                   LOG_P(PSTR("\n\nPress ENTER twice for menu... "));
                   USART0_vRXWaitForLine();
                   eState = ST_CHECK_FOR_1ST_CR;
                   EventTimerPostAfter(EVENT_TIMER_GUI_TIMEOUT, EV_GUI_TIMEOUT, 3000);
                   break;

               case ST_CHECK_FOR_1ST_CR:
               case ST_CHECK_FOR_2ND_CR:
                   switch (eEvent)
                   {
                       case  EV_UART_LINE_COMPLETE:
                           if (eState == ST_CHECK_FOR_1ST_CR)
                           {
                               eState = ST_CHECK_FOR_2ND_CR;
                               USART0_vRXWaitForLine();
                               EventTimerPostAfter(EVENT_TIMER_GUI_TIMEOUT, EV_GUI_TIMEOUT, 3000);
                           }
                           else
                           {
                               eHandler = HND_MENU;
                               EventPost(EV_SHOW_MAIN_MENU);
                           }
                           break;

                       case EV_GUI_TIMEOUT:
                           eHandler = HND_APP;
                           EventPost(EV_WAIT_FOR_PULSES);
                           USART0_RXDisable();
                           break;

                       case EV_CLOCK_1S:
                           LOG_P(PSTR("."));
                           wdt_reset();
                           break;

                       default:
                           break;
                   }
                   break;
           }
}
