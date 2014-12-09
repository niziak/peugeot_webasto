#include <avr/io.h>
#include <util/delay.h>    

#include <config.h>

void main(void)
{
    ARDUINO_LED_SETUP;
    while (1) 
    {
	ARDUINO_LED_ON;
	
	_delay_ms(200);
	
	ARDUINO_LED_OFF;
	
	_delay_ms(1000);
    }
}
