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

/**
 * ISR for PCINT0
 * PCINT[7:0] are serviced by PCINT0_vect
 */
ISR(PCINT0_vect)
{
    if (bit_is_set(PINB, PINB0))
    {
        EventPostFromIRQ(SYS_EXTINT);
    }
}



void main(void) __attribute__ ((noreturn));
void main(void)
{
	DDRB &= ~_BV(PB0); // DDR=0 = input port
	PORTB |=  _BV(PB0); // enable pull up		//TODO remove in final to prevent current consumption
	PCMSK0 |= _BV(PCINT0); // enable PCINT0
	PCICR  |= _BV(PCIE0);   // Pin Change Interrupt Enable 0

	ARDUINO_LED_SETUP;
	ARDUINO_LED_ON;

	USART0_vInit();
	LOG_P(PSTR("Build " __TIME__ " " __DATE__ "\n\n"));
	LOG_P(PSTR("Press enter for menu...\n"));
    _delay_ms(500);
    ARDUINO_LED_OFF;

    EventInit();
    //wdt_enable(WDTO_2S);
    TIMER_vInit();
    TIMER1_vInit();

    sei(); // start interrupts (timer)

	//SimulationLoop();
	for (;;)
	{
		if (0) {
			LOG_P(PSTR("Waiting for external event (power down)...\n"));
			_delay_ms(20); //give UART chance to transmit
			sei();
			// Go into very deep sleep, waiting only for external interrupt on PCINT0 (PB0)
			set_sleep_mode (SLEEP_MODE_PWR_DOWN); // wait for int (1ms timer)
			sleep_mode();
			cli();
			ulSystemTickMS = 0;
			ulSystemTickS = 0;
			RESET_TIMER0_CNT
			sei();
			LOG_P(PSTR("Back from power down mode!\n"));

		}
   		set_sleep_mode (SLEEP_MODE_IDLE); // wait for int (1ms timer)
	    sleep_mode();
		//wdt_reset();

		if (TRUE == bIsEventWaiting())
		{
		       EVENT_DEF eEvent = EventGet();
		       DEBUG_P(PSTR("\n----------------------------\n"));
		       DEBUG_T_P(PSTR("Event %d\n\n"), eEvent);
		       switch (eEvent)
		       {
		       	   case SYS_CLOCK_1S:
		       		   ARDUINO_LED_ON
		       		   break;

		       	   case SYS_EXTINT:
		       		   LOG_P(PSTR("PINB=0x%02X PINC=0x%02X PB0=%d\n"), PINB, PINC, PINB & _BV(PINB0));
		       		   break;

		       	   default:
		       		   break;
		       }
		       ARDUINO_LED_OFF
		}
		LOG_P(PSTR("%5u %5u - %5u = %5u\n"),  T1Ovs1, Capt1, Capt2, Capt2 - Capt1);
		_delay_ms(50);
	    //USART0_vSendByte (USART0_ucGetByte());
	    //LOG_P(PSTR(" Woked up!\n"));
	    //_delay_ms(1000);
	}
}
