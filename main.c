#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <util/delay.h>    
#include <stdio.h>

#include <config.h>
#include <turning_light.h>
#include "usart0.h"
#include "log.h"
#include <events.h>

void main(void) __attribute__ ((noreturn));
void main(void)
{
	ARDUINO_LED_SETUP;
	ARDUINO_LED_ON;
	_delay_ms(1000);
	USART0_vInit();
	LOG_P(PSTR("Build " __TIME__ " " __DATE__ "\n"));
    _delay_ms(500);
    ARDUINO_LED_OFF;

    EventInit();
    //wdt_enable(WDTO_2S);
    //TIMER_vInit()''
    //sei();
    DDRB &= ~_BV(PB0); // DDR=0 = input port
    PORTB |=  _BV(PB0); // enable pull up
	//SimulationLoop();
	for (;;)
	{
		//wdt_reset();
//    		set_sleep_mode (SLEEP_MODE_IDLE); // wait for int
//	    sleep_mode();
	    //USART0_vSendByte (USART0_ucGetByte());
		LOG_P(PSTR("PINB=0x%02X PINC=0x%02X PB0=%d\n"), PINB, PINC, PINB & _BV(PINB0));
		_delay_ms(100);
	    //LOG_P(PSTR(" Woked up!\n"));
	    //_delay_ms(1000);
	}
}
