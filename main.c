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


const uint16_t auiExpectedPeriodsMS[MAX_PERIODS] =
{
        /*  1 */     200, 100,
        /*  2 */     200, 100,
        /*  3 */     200, 100,
        /*  4 */     200, 100,
        /*  5 */     200, 100,
        /*  6 */     200, 100,
        /*  7 */     200, 100,
        /*  8 */     200, 100,
        /*  9 */     200, 100,
        /* 10 */     200, 100,
};


void vWaitForNextSeries(void)
{
    TIMER1_vStop();
    LOG_P(PSTR("Waiting for external event (power down)...\n"));
    _delay_ms(20); //give UART chance to transmit

    TIMER1_vInit(); // start pulse measure
    sei();
    // Go into very deep sleep, waiting only for external interrupt on PCINT0 (PB0)
    set_sleep_mode (SLEEP_MODE_PWR_DOWN); // wait for int (1ms timer)
    sleep_mode();

    cli();

    ulIdleTimeMS   = 0;
    ulSystemTickMS = 0;
    ulSystemTickS  = 0;
    RESET_TIMER0_CNT
    sei();

    LOG_P(PSTR("Line change - back from power down mode!\n"));
}


BOOL bAnalyzeCollectedPulses(void)
{
        uint8_t uiCollectedIndex, uiExpectedIndex;
        uiExpectedIndex = 0;
        BOOL bFirstMatched = FALSE;
        BOOL bFailed = FALSE;

        LOG_P(PSTR("Analyzing...\n"));
        for (uiCollectedIndex=0; uiCollectedIndex < MAX_PERIODS; uiCollectedIndex++)
        {
            uint32_t uiPeriodMS = (uint32_t)auiPeriods[uiCollectedIndex] * (uint32_t)TIMER1_TICK_US / (uint32_t)1000;
            printf("#%02d %5u ", uiCollectedIndex, uiPeriodMS);
            if (    (auiExpectedPeriodsMS[uiExpectedIndex] + PULSE_LEN_TOLERANCE_MS > uiPeriodMS)
                 && (auiExpectedPeriodsMS[uiExpectedIndex] - PULSE_LEN_TOLERANCE_MS < uiPeriodMS) )
            {
                bFirstMatched = TRUE;
                printf("=");
            }
            else
            // value not matched
            {
                if (bFirstMatched)
                {
                    // fail - sequence started, but value is not matched
                    printf("!");
                    bFailed = TRUE;
                }
                else
                {
                    printf(" ");
                }
            }

            if (bFirstMatched)
            {
                uiExpectedIndex++;
            }
            printf(" (#%02d = %5u)\n", uiExpectedIndex, auiExpectedPeriodsMS[uiExpectedIndex]);

            if (bFailed)
            {
                break;
            }
        }
        return bFailed;
}


void main(void) __attribute__ ((noreturn));
void main(void)
{
	DDRB   &= ~ _BV(PB0);     // DDR=0 = input port
	PORTB  |=   _BV(PB0);     // enable pull up		//TODO remove in final to prevent current consumption

	PIN_CHANGE_INT_ENABLE

	ARDUINO_LED_SETUP;
	ARDUINO_LED_ON;

	USART0_vInit();
	LOG_P(PSTR("Build " __TIME__ " " __DATE__ "\n\n"));
	switch (MCUSR)
	{
		case (1<<PORF):  LOG_P (PSTR("Power on\n"));      break;
		case (1<<EXTRF): LOG_P (PSTR("Ext RST\n"));       break;
		case (1<<BORF):  LOG_P (PSTR("Brown out RST\n")); break;
		case (1<<WDRF):	 LOG_P (PSTR("WDT RST\n"));       break;
		default:
			LOG_P(PSTR("MCUSR=%02X\n"), MCUSR);
			break;

	}
	LOG_P(PSTR("Press enter for menu...\n"));
    _delay_ms(500);
    ARDUINO_LED_OFF;

    EventInit();
    //wdt_enable(WDTO_2S);
    TIMER_vInit();

    ulIdleTimeMS = IDLE_WHEN_NO_PULSES_MS+1; // after power up go immediately to idle mode

    sei(); // start interrupts (timer)

    LOG_P(PSTR("Temp offset=%d gain=%d/100\n"), TEMP_SENS_OFFSET, TEMP_SENS_GAIN_100);
	//SimulationLoop();
	for (;;)
	{
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
		           case SYS_GO_TO_SLEEP:
		               //vWaitForNextSeries();
		               break;

		           case SYS_CHECK_PULSES:
		               //bAnalyzeCollectedPulses();
		               break;

		       	   case SYS_CLOCK_1S:
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

		       	       printf("Temp ADC=%d ", iTemp);
		               iTemp -= TEMP_SENS_OFFSET;
		               //iTemp /= 1.22;
		               printf("Temp=%d %d\n", iTemp, iTemp * TEMP_SENS_GAIN_100 / 100);
		       		   //ARDUINO_LED_ON
		       		   break;

//		       	   case SYS_PB0_HI:
//		       		   LOG_P(PSTR("PB0=HIGH!\n"), PINB & _BV(PINB0));
//		       		   break;

		       	   default:
		       		   break;
		       }
		       //ARDUINO_LED_OFF
		}

//		printf("\n");


		//_delay_ms(50);
	    //USART0_vSendByte (USART0_ucGetByte());
	    //LOG_P(PSTR(" Woked up!\n"));
	    //_delay_ms(1000);
	}
}
