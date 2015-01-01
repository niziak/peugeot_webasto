/*
 * pulse_det.h
 *
 *  Created on: 21 Dec 2014
 *      Author: niziak
 */

#ifndef PULSE_DET_H_
#define PULSE_DET_H_

#include "config.h"

extern void PD_vClearStoredPattern(void);
extern void PD_vWaitForNextSeries(void);
extern BOOL PD_bAnalyzeCollectedPulses(void);

extern volatile uint16_t auiPeriods[MAX_PERIODS];
extern volatile uint8_t  ucWriteIndex;

#endif /* PULSE_DET_H_ */
