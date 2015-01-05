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


extern uint16_t               uiHeaterSwitchOffAfterS;
extern  int16_t               s16Temperature;
extern uint16_t               u16CarVoltage;

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
    uint16_t                    u16IdleWhenNoPulsesMs;
    uint16_t                    u16PulseLenToleranceMs;
    uint16_t                    u16HeaterEnabledForMin;
    int8_t                      s8HeaterEnableMaxTemperature;
    uint16_t                    u16VoltageDividerRatio;  ///< in 1/100 (e.g 3.87 should be 387)
    uint16_t                    u16VoltageWithEngine;    ///< in mV voltage level when engine is running
    uint16_t                    u16VoltageMinimumLevel;  ///< in mV critical voltage level
    uint8_t                     aucSpare1[32];

    TEMP_CAL_DEF                astTempCal[TEMP_CALIB_POINTS];
    TEMP_CAL_DEF                astTempCalSpare[8];

    uint16_t                    auiExpectedPeriodsMS[MAX_PERIODS];
    uint16_t                    auiExpectedPeriodsMSSpare[MAX_PERIODS];
    uint8_t                     aucSpare[32];
} NVM_SET_DEF;


extern NVM_SET_DEF                  stSettings;
#define pstSettings (&(stSettings))


#include <avr/pgmspace.h>
extern const char copyright[] PROGMEM;

typedef enum
{
    HND_STARTUP,
    HND_APP,
    HND_MENU,
} HND_DEF;

extern HND_DEF eHandler;

typedef enum
{
    ST_CHECK_FOR_MENU_ENTER,
    ST_CHECK_FOR_1ST_CR,
    ST_CHECK_FOR_2ND_CR,

    ST_MENU_SHOW_MAIN,
    ST_MENU_WAIT_FOR_PATTERN,

} APP_ST_DEF;

extern APP_ST_DEF             eState;

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
