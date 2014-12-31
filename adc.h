/*
 * adc.h
 *
 *  Created on: 21 Dec 2014
 *      Author: niziak
 */

#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>

typedef enum
{
    ADC0 = 0,
    ADC1,
    ADC2,
    ADC3,
    ADC4,
    ADC5,
    ADC6,
    ADC7,
    ADC_TEMP,   ///< Internal temperature sensor
    /* ... */
    ADC_BG  = 0b1110,  ///< bandgap
    ADC_GND = 0b1111,
} ADC_CH_DEF;

typedef enum
{
                                             ///< REFS1 REFS0 bits
    ADC_REF_AREF = 0,                        ///<  00    external Aref
    ADC_REF_VCC  = _BV(REFS0),               ///<  01    Vcc + ext cap
    ADC_REF_INV  = _BV(REFS1),               ///<  10    invalid value
    ADC_REF_11   = _BV(REFS1) | _BV(REFS0),  ///<  11    Internal 1.1V + ext cap
} ADC_REF_DEF;

#define ADC_AVG_NUM_OF_SAMPLES 15

extern void ADC_vPrepare (ADC_CH_DEF eChannel, ADC_REF_DEF eReference);

extern void ADC_vStart(void);
extern void ADC_vStartNoiseReduction(void);

extern void ADC_vWait(void);

extern void ADC_vStop(void);

extern void ADC_vReadADCAverage(ADC_CH_DEF eChannel, ADC_REF_DEF eReference);

extern void ADC_vGetVoltage(void);


#endif /* ADC_H_ */
