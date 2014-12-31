/*
 * globals.c
 *
 *  Created on: 12 sie 2014
 *      Author: nizinski_w
 */

#include "config.h"
#include "types.h"
#include <avr/pgmspace.h>
#include <stdint.h>
#include "globals.h"
#include <string.h>

volatile uint32_t      ulSystemTickMS = 0;         ///< local time tick counter (increment every ms)
volatile uint32_t      ulSystemTickS = 0;          ///< local time tick counter (increment every second)

volatile uint16_t      uiIdleTimeMS = IDLE_TIME_DISABLED;  ///<

int16_t                iADCVal;
uint16_t               uiHeaterSwitchOffAfterS;

NVM_SET_DEF            stSettings;

HND_DEF                eHandler;    ///< what application handler is active
APP_ST_DEF             eState;      ///< handler state (shared for all handlers)

const char copyright[] PROGMEM = "\
Copyright (C) 2013-2015 Wojciech Nizinski. All rights reserved. \
You can use this work only for reference. \
No part of this work may be redistributed or modified in any form or any means (source, binary, others) \
without prior written permission from Wojciech Nizinski\n";






static const uint16_t auiExpectedPeriodsMS[MAX_PERIODS] =
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
        /* 10 */     200,
};

void NVM_vSetDefaultValues(void)
{
    memcpy (pstSettings->auiExpectedPeriodsMS, auiExpectedPeriodsMS, sizeof (auiExpectedPeriodsMS));

    pstSettings->u16IdleWhenNoPulsesMs          = IDLE_WHEN_NO_PULSES_MS;
    pstSettings->u16PulseLenToleranceMs         = PULSE_LEN_TOLERANCE_MS;
    pstSettings->u16HeaterEnabledForMin         = HEATER_ENABLED_FOR_MIN;
    pstSettings->s8HeaterEnableMaxTemperature   = HEATER_ENABLED_MAX_TEMPERATURE;

    pstSettings->astTempCal[0].s16ADCTemp       = TEMP_SENS_T1_ADC;
    pstSettings->astTempCal[0].s8RealTemp       = TEMP_SENS_T1_REAL;

    pstSettings->astTempCal[1].s16ADCTemp       = TEMP_SENS_T2_ADC;
    pstSettings->astTempCal[1].s8RealTemp       = TEMP_SENS_T2_REAL;

}

