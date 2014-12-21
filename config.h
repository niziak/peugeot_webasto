#include "types.h"


// Arduino Nano LED connected to D13/PB5/SCK and ground
#define ARDUINO_LED_SETUP	{ DDRB  |=  _BV(DDB5); }
#define ARDUINO_LED_ON		{ PORTB |=  _BV(PB5); }
#define ARDUINO_LED_OFF		{ PORTB &= ~_BV(PB5); }
#define ARDUINO_LED_ALTER	{ PORTB ^=  _BV(PB5); }


#define TIMER1_TICK_US                  64      ///< One T1 count = 64us

#define CHECK_WHEN_NO_PULSES_MS         1500    ///< analyse collected pulses after ms time
#define IDLE_WHEN_NO_PULSES_MS			3000	///< go to idle when no pulses witin 3 seconds from last pulse


#define MAX_PERIODS                     30      ///< how many periods store (positive and negative state lengths)

#define PULSE_LEN_TOLERANCE_MS          20

// Calibration from: http://www.atmel.com/images/doc8108.pdf
#define TEMP_SENS_T1_ADC                336
#define TEMP_SENS_T1_REAL               10

#define TEMP_SENS_T2_ADC                356
#define TEMP_SENS_T2_REAL               26

#define TEMP_SENS_ADC_DIST              (TEMP_SENS_T2_ADC  - TEMP_SENS_T1_ADC)
#define TEMP_SENS_REAL_DIST             (TEMP_SENS_T2_REAL - TEMP_SENS_T1_REAL)

#define TEMP_SENS_OFFSET                ((TEMP_SENS_ADC_DIST/TEMP_SENS_REAL_DIST)-TEMP_SENS_T1_REAL+TEMP_SENS_T1_ADC)
#define TEMP_SENS_GAIN_100              (TEMP_SENS_T2_REAL*100)/((TEMP_SENS_T2_ADC)-(TEMP_SENS_OFFSET))

