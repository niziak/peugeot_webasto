#include <avr/io.h>
#include <util/delay.h>
#include <config.h>

#define TURNING_LIGHT_NUM_OF_CYCLES	5
#define TURNING_LIGHT_ON_TIME_MS	400
#define TURNING_LIGHT_OFF_TIME_MS	200


/**
 * Generate one sequence of Peugeot turning light
 * @param uiSpreadMs - make random some error in timing (+- uiSpreadMs)
 */
void SimulateTurningLight (const unsigned int uiSpreadMs)
{
	for (unsigned char i=0; i<TURNING_LIGHT_NUM_OF_CYCLES; i++)
	{
		ARDUINO_LED_ON
		_delay_ms(TURNING_LIGHT_ON_TIME_MS);
		ARDUINO_LED_OFF
		_delay_ms(TURNING_LIGHT_OFF_TIME_MS);
	}
}

#define LOCATING_LIGHT_NUM_OF_CYCLES	10
#define LOCATING_LIGHT_ON_TIME_MS		100
#define LOCATING_LIGHT_OFF_TIME_MS		100
void SimulateLocatingVehicle (const unsigned int uiSpreadMs)
{
	for (unsigned char i=0; i<LOCATING_LIGHT_NUM_OF_CYCLES; i++)
	{
		ARDUINO_LED_ON
		_delay_ms(LOCATING_LIGHT_ON_TIME_MS);
		ARDUINO_LED_OFF
		_delay_ms(LOCATING_LIGHT_OFF_TIME_MS);
	}
}


void SimulationLoop(void)  __attribute__ ((noreturn));
void SimulationLoop(void)
{
	while (1)
	{
		SimulateTurningLight(0);
		_delay_ms(5000);
		SimulateLocatingVehicle(0);
		_delay_ms(5000);
	}
}
