/*
 * timer1.h
 *
 *  Created on: Dec 18, 2014
 *      Author: niziak
 */

#ifndef TIMER1_H_
#define TIMER1_H_
#include "config.h"

extern void TIMER1_vInit(void);
extern void TIMER1_vStop(void);

extern volatile uint16_t auiPeriods[MAX_PERIODS];


#endif /* TIMER1_H_ */
