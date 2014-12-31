/*
 * adc.c
 *
 *  Created on: 21 Dec 2014
 *      Author: niziak
 */

#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#include "types.h"
#include "globals.h"
#include "adc.h"
#include "log.h"
#include "usart0.h"

volatile uint16_t      u16ADCVal;

ISR(ADC_vect)
{
    // read ADCL first then ADCH
    u16ADCVal  = ADCL;
	u16ADCVal |= (ADCH<<8);
}



/**
 * Prepare ADC at given by @ref eChannel for conversion
 * @param eChannel - see @ref ADC_CH_DEF
 */
void ADC_vPrepare (ADC_CH_DEF eChannel,  ADC_REF_DEF eReference)
{
    ADMUX = (   eReference
              | (0<<ADLAR)
              | eChannel
              );

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
    while (bit_is_set(ADCSRA,ADSC)); // wait for end of conversion
}

void ADC_vWait(void)
{
    while (bit_is_set  (ADCSRA,ADSC)); // wait for end of conversion
    while (bit_is_clear(ADCSRA,ADIF)); // wait for interrupt processing complete
}

void ADC_vStop(void)
{
    ADCSRA = 0;
}

/**
 * Measure value at given ADC channel. Result is stored in global variable @ref iADCVal
 * Multiple ADC reads are performed and average value is given
 * @param eChannel
 */
void ADC_vReadADCAverage(ADC_CH_DEF eChannel, ADC_REF_DEF eReference)
{
    uint16_t uiAvg=0;
    ADC_vPrepare(eChannel, eReference);
    for (uint8_t i=1; i<ADC_AVG_NUM_OF_SAMPLES+1; i++)
    {
        //DEBUG_T_P(PSTR("Start ADC%d R%02X ... "), eChannel, eReference); USART0_vFlush();
        #if (0)
          ADC_vStart();
          ADC_vWait();
        #else
          ADC_vStartNoiseReduction();
        #endif
        uiAvg  = (uiAvg * (i-1)); // restore total value from previous samples
        uiAvg += u16ADCVal;         // add current sample
        uiAvg /= i;               // divide by number of current sample
        //DEBUG_P(PSTR("ADC=%d avg=%d\n"), iADCVal, uiAvg);
    }
    ADC_vStop();
    u16ADCVal = uiAvg;
}

void ADC_vGetCarVoltage(void)
{
    // switch port A2/PC2 to LOW, to enable voltage divider
    //VOLTAGE_DIVIDER_ENABLE
    ADC_vReadADCAverage(ADC3, ADC_REF_VCC);
    //VOLTAGE_DIVIDER_DISABLE

    uint32_t u32Temp = 5000UL*u16ADCVal / 1023UL; // store result in mV (5V=5000mV)
    // now in u32Temp is real voltage on ADC3 input
    // multiply it by voltage divider factor (R1=22k R2=7,5K)

    //TODO calibration!
    //u16CarVoltage = ( 7500UL / (22000UL+7500UL) ) / u32Temp;
    //u16CarVoltage = (7500UL) / (u32Temp*(22000UL+7500UL));

    //u16CarVoltage = u32Temp * 387UL / 100UL; // WORKS: 3.87 is real factor of my divider
    u16CarVoltage = u32Temp * stSettings.u16VoltageDividerRatio / 100UL; // 3.87 is real factor of my divider

    LOG_P(PSTR("Car voltage: %d mV (RAW=%d)\n"), u16CarVoltage, u16ADCVal);
}
