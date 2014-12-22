/*
 * adc.c
 *
 *  Created on: 21 Dec 2014
 *      Author: niziak
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "globals.h"


ISR(ADC_vect)
{
    // read ADCL first then ADCH
    iTemp  = ADCL;
	iTemp |= (ADCH<<8);
}

void ADC_vPrepare(void)
{
    ADMUX = (   (1<<REFS1) | (1<<REFS0) /* 1.1V reference voltage for temp sensor*/
              | (0<<ADLAR)
              | (1<<MUX3) );            /* 1000 - ADC8 - temp sensor */

    ADCSRA = (   (1<<ADEN)      /* ADC enable - set voltages*/
               | (1<<ADIE)      /* ADC INT enabled */
               | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) /*/128 prescaler */
             );

    _delay_ms(20); // wait for ext capacitor to be charged
}


void ADC_vStart(void)
{
	ADCSRA |= (1<<ADSC); /* Start conversion NOW */
}

void ADC_vStartNoiseReduction(void)
{
    // it will automatically start ADC single conversion afte halting CPU
    set_sleep_mode (SLEEP_MODE_ADC); // wait for ADC complete int or other int
    sleep_mode();
}

void ADC_vWait(void)
{
    while (bit_is_set(ADCSRA,ADSC)); // wait for end of conversion
    while (bit_is_clear(ADCSRA,ADIF)); // wait for interrupt processing complete
}

void ADC_vStop(void)
{
    ADCSRA = 0;
}
