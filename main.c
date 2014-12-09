#include <avr/io.h>
#include <util/delay.h>    

#include <config.h>
#include <turning_light.h>

void main(void)
{
    ARDUINO_LED_SETUP;
    while (1) 
    {
    	SimulateTurningLight(0);
    	_delay_ms(5000);
    	SimulateLocatingVehicle(0);
    	_delay_ms(5000);
    }
}
