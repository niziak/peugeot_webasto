#include "types.h"


// Arduino Nano LED connected to D13/PB5/SCK and ground
#define ARDUINO_LED_SETUP	{ DDRB  |=  _BV(DDB5); }
#define ARDUINO_LED_ON		{ PORTB |=  _BV(PB5); }
#define ARDUINO_LED_OFF		{ PORTB &= ~_BV(PB5); }
#define ARDUINO_LED_ALTER	{ PORTB ^=  _BV(PB5); }


#define TIMER0_ISR_EVERY_US	1000
