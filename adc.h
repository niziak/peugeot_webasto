/*
 * adc.h
 *
 *  Created on: 21 Dec 2014
 *      Author: niziak
 */

#ifndef ADC_H_
#define ADC_H_

extern void ADC_vPrepare(void);

extern void ADC_vStart(void);
extern void ADC_vStartNoiseReduction(void);

extern void ADC_vWait(void);

extern void ADC_vStop(void);


#endif /* ADC_H_ */
