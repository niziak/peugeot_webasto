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

/**
 * ISR for PCINT0
 * PCINT[7:0] are serviced by PCINT0_vect
 */
ISR(PCINT0_vect)
{
    if (bit_is_set(PINB, PINB0))
    {
    	ulIdleTimeMS = 0;
        //EventPostFromIRQ(SYS_PB0_HI);
    }
}


#define PIN_CHANGE_INT_ENABLE		{ PCICR  |=   _BV(PCIE0); PCMSK0 |=   _BV(PCINT0); }
#define PIN_CHANGE_INT_DISABLE		{ PCICR  &= ~ _BV(PCIE0); PCMSK0 &= ~ _BV(PCINT0); }

uint8_t mcusr __attribute__ ((section(".noinit")));
void vGetOptibootMCUSR(void) __attribute__((naked)) __attribute((section(".init()")));
void vGetOptibootMCUSR(void)
{
    __asm__ __volatile__ ( "mov %0, r2 \n" : "=r" (mcusr) : );
}

void main(void) __attribute__ ((noreturn));
void main(void)
{

    WEBASTO_PIN_SETUP;
    HEATER_OFF;

    ARDUINO_LED_SETUP;
    ARDUINO_LED_ON;

	DDRB   &= ~ _BV(PB0);     // DDR=0 = input port
	PORTB  |=   _BV(PB0);     // enable pull up		//TODO remove in final to prevent current consumption

	PIN_CHANGE_INT_ENABLE

	USART0_vInit();
	LOG_P(PSTR("Build " __TIME__ " " __DATE__ "\n\n"));
	LOG_P(PSTR("Reset reason: "));
	switch (mcusr)
	{
		case (1<<PORF):  LOG_P (PSTR("Power on\n"));      break;
		case (1<<EXTRF): LOG_P (PSTR("Ext RST\n"));       break;
		case (1<<BORF):  LOG_P (PSTR("Brown out RST\n")); break;
		case (1<<WDRF):	 LOG_P (PSTR("WDT RST\n"));       break;
		default:
		    LOG_P(PSTR("%02X\n"), mcusr);
			break;

	}

    EventInit();

	LOG_P(PSTR("Press enter for menu...\n"));

	_delay_ms(500);
    ARDUINO_LED_OFF;


    wdt_enable(WDTO_2S);
    TIMER0_vInit();

    sei(); // start interrupts (timer)

    LOG_P(PSTR("Temp offset=%d gain=%d/100\n"), TEMP_SENS_OFFSET, TEMP_SENS_GAIN_100);
	//SimulationLoop();
    EventPost(EV_WAIT_FOR_PULSES);
    uiHeaterSwitchOffAfter = 0;
	for (;;)
	{
	    set_sleep_mode (SLEEP_MODE_IDLE); // wait for event (INT from 1ms timer)
	    sleep_mode();

		if (TRUE == bIsEventWaiting())
		{
		       EVENT_DEF eEvent = EventGet();
		       DEBUG_P(PSTR("\n----------------------------\n"));
		       DEBUG_T_P(PSTR("Event %d\n\n"), eEvent);

		       switch (eEvent)
		       {
		           case EV_WAIT_FOR_PULSES:
		               uiHeaterSwitchOffAfter = 0;
		               vWaitForNextSeries();
		               break;

		           case EV_CHECK_PATTERN:
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
                       LOG_P(PSTR("Pattern match.\n"));
                       EventPost(EV_READ_TEMPERATURE);
                       break;

		           case EV_READ_TEMPERATURE:
		               LOG_P(PSTR("Reading ambient temperature... "));
		               ADC_vPrepare();

		                       #if (0)
		                         ADC_vStart();
		                         ADC_vWait();
		                       #else
		                         ADC_vStartNoiseReduction();
		                         while (bit_is_set(ADCSRA,ADSC)); // wait for end of conversion
		                       #endif

		                       ADC_vStop();

		                       //avg+=(iTemp-avg)/(float)i;

		                       printf_P(PSTR("RAW ADC=%d "), iTemp);
		                       iTemp -= TEMP_SENS_OFFSET;
		                       //iTemp /= 1.22;
		                       printf_P(PSTR("Temp=%d %d\n"), iTemp, iTemp * TEMP_SENS_GAIN_100 / 100);
		               if ((iTemp * TEMP_SENS_GAIN_100 / 100) > 35)
		               {
		                       EventPost(EV_WAIT_FOR_PULSES);
		               }
		               else
		               {
		                       EventPost(EV_START_WEBASTO);
		               }
		               break;

		           case EV_START_WEBASTO:
		               LOG_P(PSTR("Starting heater!\n"));
		               uiHeaterSwitchOffAfter = HEATER_ENABLED_FOR;
		               break;

		       	   case SYS_CLOCK_1S:
		       	       wdt_reset();
		       	       if (uiHeaterSwitchOffAfter>0)
		       	       {
		       	           uiHeaterSwitchOffAfter--;
		       	           LOG_P(PSTR("\tHeater enabled for %d sec.\n"), uiHeaterSwitchOffAfter);
		       	           if (uiHeaterSwitchOffAfter==0)
		       	           {
		       	            LOG_P(PSTR("\tStopping heater!\n"), uiHeaterSwitchOffAfter);
		       	               EventPost(EV_WAIT_FOR_PULSES);
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


		       	   default:
		       	       RESET_P(PSTR("unh event!"));
		       		   break;
		       } //switch (eEvent)
		       //ARDUINO_LED_OFF
		} // if (TRUE == bIsEventWaiting())

        if (uiHeaterSwitchOffAfter>0)
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
