/*
 * temperature.c
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#include <stdio.h>

#include <util/delay.h>

#include "config.h"
#include "adc.h"
#include "globals.h"
#include "log.h"
#include "usart0.h"

static int16_t  s16Gain100; ///< Gain value multiplied by 100
static int16_t  s16Offset;  ///< Temp offset

// Calibration idea from: http://www.atmel.com/images/doc8108.pdf
#define TEMP_SENS_ADC_DIST              (TEMP_SENS_T2_ADC  - TEMP_SENS_T1_ADC)
#define TEMP_SENS_REAL_DIST             (TEMP_SENS_T2_REAL - TEMP_SENS_T1_REAL)

#define TEMP_SENS_OFFSET                ((TEMP_SENS_ADC_DIST/TEMP_SENS_REAL_DIST)-TEMP_SENS_T1_REAL+TEMP_SENS_T1_ADC)
#define TEMP_SENS_GAIN_100              (TEMP_SENS_T2_REAL*100)/((TEMP_SENS_T2_ADC)-(TEMP_SENS_OFFSET))



static int16_t s16CalcOffset (TEMP_CAL_DEF *pstP1, TEMP_CAL_DEF *pstP2)
{
    #define ADC_DELTA  (pstP2->s16ADCTemp - pstP1->s16ADCTemp)
    #define REAL_DELTA (pstP2->s8RealTemp - pstP1->s8RealTemp)
    return ( (ADC_DELTA/REAL_DELTA) - pstP1->s8RealTemp + pstP1->s16ADCTemp);
}

static int16_t s16CalcGain100 (TEMP_CAL_DEF *pstP2, int16_t s16Offset)
{
    return ( (pstP2->s8RealTemp*100) / (pstP2->s16ADCTemp - s16Offset) );
}


void TEMP_vPrintCalibrationData(void)
{
    LOG_P(PSTR("Temperature sensor calibration points:\n"));
    for (uint8_t i=0; i<TEMP_CALIB_POINTS; i++)
    {
        LOG_P(PSTR("\t#%d RAW=%3d   REAL=%2d\n"), i, pstSettings->astTempCal[i].s16ADCTemp,
                                                     pstSettings->astTempCal[i].s8RealTemp   );
    }
}

void TEMP_vCalculateCalibration(void)
{
    TEMP_vPrintCalibrationData();
    s16Offset  = s16CalcOffset  (&pstSettings->astTempCal[0], &pstSettings->astTempCal[1]);
    s16Gain100 = s16CalcGain100 (&pstSettings->astTempCal[1], s16Offset);

    LOG_P(PSTR("Calibration offset=%d gain=%d/100\n"), s16Offset, s16Gain100);
}

void TEMP_vReadTemperature(void)
{
    ADC_vPrepare();
    uint16_t uiAvg=0;
    for (uint8_t i=1; i<15+1; i++)
    {
        //DEBUG_T_P(PSTR("Start ADC... "));
        USART0_vFlush();
        #if (0)
          ADC_vStart();
          ADC_vWait();
        #else
          ADC_vStartNoiseReduction();
        #endif
        uiAvg = (uiAvg*(i-1)); // restore total value from previous samples
        uiAvg+= iTemp; // add current sample
        uiAvg/= i; // divide by number of current sample
        //DEBUG_P(PSTR("ADC=%d avg=%d\n"), iTemp, uiAvg);
    }
    ADC_vStop();

    LOG_P(PSTR("Temperature: (RAW=%d) "), iTemp);

    iTemp -= TEMP_SENS_OFFSET;
    iTemp *= TEMP_SENS_GAIN_100;
    iTemp /= 100;

    LOG_P(PSTR("REAL=%d\n"), iTemp);
}




