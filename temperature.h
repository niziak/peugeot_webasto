/*
 * temperature.h
 *
 *  Created on: 23 gru 2014
 *      Author: nizinski_w
 */

#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_


extern void TEMP_vPrintCalibrationData(void);
extern void TEMP_vReadCalibrationDataFromConsole(void);
extern void TEMP_vCalculateCalibration(void);
extern void vReadTemperature(void);

#endif /* TEMPERATURE_H_ */
