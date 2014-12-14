/*
 * globals.c
 *
 *  Created on: 12 sie 2014
 *      Author: nizinski_w
 */

#include "config.h"
#include "types.h"

OW_NEW_DEVICE_DEF           atdNewTempSensors  [NUM_OF_TEMP_SENSORS];
TEMP_SENSOR_PARAMS_DEF      atdKnownTempSensors[NUM_OF_TEMP_SENSORS];
INT                         aiPreviousTemp     [NUM_OF_TEMP_SENSORS];   ///< store integer part of previous value

volatile unsigned long      ulSystemTickMS = 0;         ///< local time tick counter (increment every ms)
volatile unsigned long      ulSystemTickS = 0;          ///< local time tick counter (increment every second)

volatile BOOL               bBlinkState;                ///< alternating variable to control blinking characters
volatile BOOL               bNeedsBlinking;             ///< flag to turn on blinking flag
volatile BOOL               bRefreshDisplay;            ///< flag to redraw display

volatile UCHAR              ucUIInactiveCounter;        ///< in seconds. Counts down

volatile unsigned int       uiPumpSwitchOffAfter;       ///< 0 - stop pump, automatically decremented every 1S in timer
volatile BOOL               bPumpIsRunning;             ///< flag to start/stop pump

volatile unsigned int       uiPIRTTL;                   ///< >0 if presence was detected (decremented every 1S in timer)

#if (WITH_RTC_DRIFT_MEASURE)
         long              lCalcTimeOfs;               ///< calculated RTC time offset
#endif
NVM_SET_DEF                 stSettings;

const CHAR copyright[] PROGMEM = "\
Copyright (C) 2013-2014 Wojciech Nizinski. All rights reserved. \
You can use this work only for reference. \
No part of this work may be redistributed or modified in any form or any means (source, binary, others) without prior written permission from Wojciech Nizinski\n";






