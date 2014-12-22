#include "types.h"


// Arduino Nano LED connected to D13/PB5/SCK and ground
#define ARDUINO_LED_SETUP	{ DDRB  |=  _BV(DDB5); }
#define ARDUINO_LED_ON		{ PORTB |=  _BV(PB5); }
#define ARDUINO_LED_OFF		{ PORTB &= ~_BV(PB5); }
#define ARDUINO_LED_ALTER	{ PORTB ^=  _BV(PB5); }

// Webasto start is D9  (PB1 OC1A / PCINT1)
#define WEBASTO_PIN_SETUP   { DDRB  |=  _BV(DDB1); }
#define WEBASTO_PIN_ON      { PORTB |=  _BV(PB1); }
#define WEBASTO_PIN_OFF     { PORTB &= ~_BV(PB1); }
#define WEBASTO_PIN_ALTER   { PORTB ^=  _BV(PB1); }

#define HEATER_ON           WEBASTO_PIN_OFF
#define HEATER_OFF          WEBASTO_PIN_ON

#define TIMER1_TICK_US                  64      ///< One T1 count = 64us

/**
 * Not used. Currently Timer1 overflow is used for maximum pulse time or idle time (4.19 second)
 */
//#define IDLE_WHEN_NO_PULSES_MS			2000	///< maximum pulse time in ms (max 4000!)


#define MAX_PERIODS                     30      ///< how many periods store (positive and negative state lengths)

#define PULSE_LEN_TOLERANCE_MS          20

// Calibration idea from: http://www.atmel.com/images/doc8108.pdf
#define TEMP_SENS_T1_ADC                336
#define TEMP_SENS_T1_REAL               10

#define TEMP_SENS_T2_ADC                356
#define TEMP_SENS_T2_REAL               26

#define TEMP_SENS_ADC_DIST              (TEMP_SENS_T2_ADC  - TEMP_SENS_T1_ADC)
#define TEMP_SENS_REAL_DIST             (TEMP_SENS_T2_REAL - TEMP_SENS_T1_REAL)

#define TEMP_SENS_OFFSET                ((TEMP_SENS_ADC_DIST/TEMP_SENS_REAL_DIST)-TEMP_SENS_T1_REAL+TEMP_SENS_T1_ADC)
#define TEMP_SENS_GAIN_100              (TEMP_SENS_T2_REAL*100)/((TEMP_SENS_T2_ADC)-(TEMP_SENS_OFFSET))

#define HEATER_ENABLED_FOR              (5*60) ///< 15 minutes
