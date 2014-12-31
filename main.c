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
#include "handler_startup.h"


#define DEBUG_EVENTS    0


/**
 * Get original reset reason from Optiboot.
 * Optiboot bootloader should save original MCUSR into R2
 */
uint8_t mcusr __attribute__ ((section(".noinit")));
void vGetOptibootMCUSR(void) __attribute__((naked)) __attribute((section(".init()")));
void vGetOptibootMCUSR(void)
{
    __asm__ __volatile__ ( "mov %0, r2 \n" : "=r" (mcusr) : );
}

void main(void) __attribute__ ((noreturn));
void main(void)
{
    WdtEnable();
    MCUSR |= _BV(PUD); // disable all pull-ups
    // after power up, all ports are tri-state: DDR=0 PORT=0
    // to prevent from floating, better is to set all ports to LOW
    //
    // to put pin into HI state, first change DDR=1 then PORT=1

    DDRB = 0xFF; PORTB=0;
    DDRC = 0xFF; PORTC=0;
    DDRD = 0xFF; PORTD=0;
    DIDR0 = 0xFF; // digital input buffer disable on ADC
    DIDR1 = 0xFF; // digital input buffer disable on analog comparator

    // RX and TX into Tri state mode PD0 PD1
    DDRD &= ~( _BV(DDD0) | _BV(DDD1) );


    VOLTAGE_ADC_PIN_SETUP;
    VOLTAGE_DIVIDER_PIN_SETUP;
    VOLTAGE_DIVIDER_ENABLE;

    WEBASTO_PIN_SETUP;
    HEATER_OFF;

    ARDUINO_LED_SETUP;
    ARDUINO_LED_ON;

    WEBASTO_STATE_PIN_SETUP;

	// PB0 into Input Tri-state (pin is externally pulled down by voltage divider)
    DDRB   &= ~ _BV(PB0);     // DDR=0 = input port
    PORTB  &= ~ _BV(PB0);     // PORT=0 = disable pull-up

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
    EventPost(EV_CLOCK_1S); // first generic event can be used as startup notification
    sei(); // start interrupts (especially timer)

    ARDUINO_LED_OFF;

    TEMP_vCalculateCalibration();

    //SimulationLoop();
    eState = ST_CHECK_FOR_MENU_ENTER;
    eHandler = HND_STARTUP;

    uiHeaterSwitchOffAfterS = 0;
	for (;;)
	{
	    set_sleep_mode (SLEEP_MODE_IDLE); // wait for event (INT from 1ms timer)
	    sleep_mode();

		if (TRUE == bIsEventWaiting())
		{
		       EVENT_DEF eEvent = EventGet();
               #if (DEBUG_EVENTS)
		           DEBUG_P(PSTR("\n----------------------------\n"));
		           DEBUG_T_P(PSTR("Event 0x%02X\n\n"), eEvent);
               #endif

		       switch (eHandler)
		       {
		           case HND_STARTUP:
		               STARTUP_vHandleEvent(eEvent);
		               break;

		           case HND_APP:
		               APP_vHandleEvent(eEvent);
		               break;

		           case HND_MENU:
		               MENU_vHandleEvent(eEvent);
		               break;

		       }

		} // if (TRUE == bIsEventWaiting())

        if (uiHeaterSwitchOffAfterS>0)
        {
            HEATER_ON
        }
        else
        {
            HEATER_OFF
        }
//		printf("\n");


		//_delay_ms(50);
	    //USART0_vSendByte (USART0_ucGetByte());
	    //LOG_P(PSTR(" Woked up!\n"));
	    //_delay_ms(1000);
	}
}
