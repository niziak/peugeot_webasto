/*
 * globals.h
 *
 *  Created on: 12 sie 2014
 *      Author: nizinski_w
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

extern volatile unsigned long       ulSystemTickMS;
extern volatile unsigned long       ulSystemTickS;

#include <avr/pgmspace.h>
extern const char copyright[] PROGMEM;

#if 0

/** Structure to keep whole system settings */
typedef struct
{
    UINT                        uiPumpManualTime;           ///< (in seconds) pump running time activated by user (manual mode)
    UINT                        uiPumpPIRTime;              ///< (in seconds) pump running time activated from PIR sensor
    UCHAR                       ucMinTempZasobnik;          ///< (in celsius) [0..99]
    UCHAR                       ucMinTempKran;              ///< (in celsius) [0..99]
    UCHAR                       ucMaxTempKran;              ///< (in celsius) [0..99]
    signed char                 cSecondsPerDayAdj;          ///< (in seconds) [-20..20] daily adjustment for RTC clock (@ref SEC_PER_DAY_ADJ_MIN and @ref SEC_PER_DAY_ADJ_MAX)
    UINT                        uiBacklightTime;            ///< (in seconds) back light time
    UCHAR                       aucSpare[14];

} NVM_SET_DEF;


extern NVM_SET_DEF                  stSettings;
#define pstSettings (&(stSettings))



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
