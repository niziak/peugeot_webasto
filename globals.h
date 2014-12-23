/*
 * globals.h
 *
 *  Created on: 12 sie 2014
 *      Author: nizinski_w
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

extern volatile uint32_t      ulSystemTickMS;
extern volatile uint32_t      ulSystemTickS;
extern volatile uint16_t      uiIdleTimeMS;

extern int16_t                iTemp;
extern uint16_t               uiHeaterSwitchOffAfterS;

extern BOOL                   bInMenu;

#define IDLE_TIME_DISABLED  0xFFFF

#include "config.h"

/**
 * Structure describes single point of temp sensor calibration
 */
typedef struct
{
    int8_t   s8RealTemp;
    int16_t  s16ADCTemp;
} TEMP_CAL_DEF;

/** Structure to keep whole system settings */
typedef struct
{
    uint16_t                    auiExpectedPeriodsMS[MAX_PERIODS];

    uint16_t                    u16IdleWhenNoPulsesMs;
    uint16_t                    u16PulseLenToleranceMs;
    uint16_t                    u16HeaterEnabledForMin;
    uint8_t                     u8HeaterEnableMaxTemperature;

    TEMP_CAL_DEF                astTempCal[TEMP_CALIB_POINTS];

    uint8_t                     aucSpare[32];
} NVM_SET_DEF;


extern NVM_SET_DEF                  stSettings;
#define pstSettings (&(stSettings))


#include <avr/pgmspace.h>
extern const char copyright[] PROGMEM;

#if 0




extern volatile BOOL                bBlinkState;
extern volatile UCHAR               ucUIInactiveCounter;
extern volatile unsigned int        uiPumpSwitchOffAfter;
extern volatile BOOL                bRefreshDisplay;
extern volatile BOOL                bNeedsBlinking;
extern volatile unsigned int        uiPIRTTL;
extern volatile BOOL                bPumpIsRunning;
#if (WITH_RTC_DRIFT_MEASURE)
extern          long                lCalcTimeOfs;
#endif



#define DISP_REFRESH                { bRefreshDisplay = TRUE;  }
#define DISP_START_BLINK_TIMER      { bNeedsBlinking  = TRUE;  }
#define DISP_STOP_BLINK_TIMER       { bNeedsBlinking  = FALSE; }

#define ABS_DIFF(x,y)               ((x)>(y) ? (x) - (y) : (y) - (x))
#endif

#endif /* GLOBALS_H_ */
