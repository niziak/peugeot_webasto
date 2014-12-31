#include "types.h"


// Arduino Nano LED connected to D13/PB5/SCK and ground
#define ARDUINO_LED_SETUP	{ DDRB  |=  _BV(DDB5); }
#define ARDUINO_LED_ON		{ PORTB |=  _BV(PB5);  }
#define ARDUINO_LED_OFF		{ PORTB &= ~_BV(PB5);  }
#define ARDUINO_LED_ALTER	{ PORTB ^=  _BV(PB5);  }

// Webasto start is D9  (PB1 OC1A / PCINT1)
#define WEBASTO_PIN_SETUP   { DDRB  |=  _BV(DDB1); }
#define WEBASTO_PIN_ON      { PORTB |=  _BV(PB1);  }
#define WEBASTO_PIN_OFF     { PORTB &= ~_BV(PB1);  }
#define WEBASTO_PIN_ALTER   { PORTB ^=  _BV(PB1);  }

#define HEATER_ON           { ARDUINO_LED_ON;  WEBASTO_PIN_ON;  }
#define HEATER_OFF          { ARDUINO_LED_OFF; WEBASTO_PIN_OFF; }

// state of Webasto ECU line is on (D2 / PD2)
#define WEBASTO_STATE_PIN_SETUP   { DDRD  &=  ~_BV(DDD2); }
#define WEBASTO_STATE_PIN_GET     ( PIND  & _BV(PD2) )
#define WEBASTO_STATE_GET         ((WEBASTO_STATE_PIN_GET==0) ? 1 : 0 )
#define WEBASTO_STATE_GET_STR     ((WEBASTO_STATE_PIN_GET==0) ? "ON" : "OFF" )

// PC2 is connected to lower resistor of voltage divider
#define VOLTAGE_DIVIDER_ENABLE      { PORTC &= ~ _BV(PC2); DDRC |= _BV(DDC2); }    // Output LOW  PORT=0 DDR=1
#define VOLTAGE_DIVIDER_DISABLE     { DDRC  &= ~ _BV(DDC2); PORTC &= ~ _BV(PC2); } // Tri-state   DDR=0 PORT=0
#define VOLTAGE_DIVIDER_PIN_SETUP   { VOLTAGE_DIVIDER_DISABLE } // Tri State

#define VOLTAGE_ADC_PIN_SETUP       { DDRC  &= ~ _BV(DDC3); PORTC &= ~ _BV(PC3); } // Tri state

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
#define PULSE_LEN_TOLERANCE_MS          50       ///< maximum pulse length difference (plus/ minux)
#define HEATER_ENABLED_FOR_MIN          15       ///< 15 minutes
#define HEATER_ENABLED_MAX_TEMPERATURE  15       ///< in Celsius
#define VOLTAGE_DIVIDER_RATIO           384      ///< in 1/100
#define VOLTAGE_WITH_ENGINE             14000    ///< in mV
#define VOLTAGE_MINIMUM_LEVEL           11000    ///< in mV


#define TEMP_SENS_T1_ADC                336
#define TEMP_SENS_T1_REAL               10

#define TEMP_SENS_T2_ADC                358
#define TEMP_SENS_T2_REAL               25
// offset=327 gain 89/100



