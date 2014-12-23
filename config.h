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

#define MAX_PERIODS                     30      ///< how many periods store (positive and negative state lengths)

#define TEMP_CALIB_POINTS               2       ///< has to be 2!
#define MENU_COUNTDOWN                  3

#define UI_TO_S                         60      ///< (uint8_t)

/// Default factory settings

/**
 * Timeout without PIN change interrupt (no pulses received)
 * NOTE: Also Timer1 overflow is used for maximum pulse time or idle time (4.19 second)
 */
#define IDLE_WHEN_NO_PULSES_MS          2000     ///< maximum pulse time in ms (max 4000!)
#define PULSE_LEN_TOLERANCE_MS          20       ///< maximum pulse length difference (plus/ minux)
#define HEATER_ENABLED_FOR_MIN          15       ///< 15 minutes
#define HEATER_ENABLED_MAX_TEMPERATURE  10       ///< in Celsius



#define TEMP_SENS_T1_ADC                336
#define TEMP_SENS_T1_REAL               10

#define TEMP_SENS_T2_ADC                358
#define TEMP_SENS_T2_REAL               25




