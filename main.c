#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>    
#include <stdio.h>

#include <config.h>
#include <turning_light.h>
#include "usart0.h"
#include "timer0.h"
#include "timer1.h"
#include "log.h"
#include <events.h>
#include "globals.h"
#include "tools.h"
#include "adc.h"
#include "pulse_det.h"
#include <nvm.h>
#include "temperature.h"
#include "handler_menu.h"
#include "handler_app.h"




uint8_t mcusr __attribute__ ((section(".noinit")));
void vGetOptibootMCUSR(void) __attribute__((naked)) __attribute((section(".init()")));
void vGetOptibootMCUSR(void)
{
    __asm__ __volatile__ ( "mov %0, r2 \n" : "=r" (mcusr) : );
}

void main(void) __attribute__ ((noreturn));
void main(void)
{
    wdt_enable(WDTO_2S);

    WEBASTO_PIN_SETUP;
    HEATER_OFF;

    ARDUINO_LED_SETUP;
    ARDUINO_LED_ON;

	DDRB   &= ~ _BV(PB0);     // DDR=0 = input port
	PORTB  |=   _BV(PB0);     // enable pull up		//TODO remove in final to prevent current consumption

	USART0_vInit();

	LOG_P(PSTR("Build " __TIME__ " " __DATE__ "\n"));

	LOG_P(PSTR("Reset reason: 0x%02X "), mcusr);
	if (mcusr & _BV(PORF) ) LOG_P (PSTR("PWR "));
	if (mcusr & _BV(EXTRF)) LOG_P (PSTR("RST "));
	if (mcusr & _BV(BORF) ) LOG_P (PSTR("BRW "));
	if (mcusr & _BV(WDRF) ) LOG_P (PSTR("WDT "));
	LOG_NL;

	NVM_vLoadSettings();
    EventInit();
    TIMER0_vInit();
    sei(); // start interrupts (especially timer)

    if (MENU_bCheckEnterSequence())
    {
        bInMenu = TRUE;
        MENU_vExecuteMenu();
        WdtResetHW();
    }

    ARDUINO_LED_OFF;

    TEMP_vCalculateCalibration();

	//SimulationLoop();
    EventPost(EV_WAIT_FOR_PULSES);
    uiHeaterSwitchOffAfterS = 0;
	for (;;)
	{
	    set_sleep_mode (SLEEP_MODE_IDLE); // wait for event (INT from 1ms timer)
	    sleep_mode();

		if (TRUE == bIsEventWaiting())
		{
		       EVENT_DEF eEvent = EventGet();
		       DEBUG_P(PSTR("\n----------------------------\n"));
		       DEBUG_T_P(PSTR("Event 0x%02X\n\n"), eEvent);

	           APP_vHandleEvent(eEvent);

		} // if (TRUE == bIsEventWaiting())

        if (uiHeaterSwitchOffAfterS>0)
        {
            HEATER_ON
            ARDUINO_LED_ON
        }
        else
        {
            HEATER_OFF
            ARDUINO_LED_OFF
        }
//		printf("\n");


		//_delay_ms(50);
	    //USART0_vSendByte (USART0_ucGetByte());
	    //LOG_P(PSTR(" Woked up!\n"));
	    //_delay_ms(1000);
	}
}
