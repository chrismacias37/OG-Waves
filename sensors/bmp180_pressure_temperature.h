/*
 * bmp180_pressure_temperature.h
 *	Description:
 *	   Retrieve and transmit data to the Bosh BMP180 temperature/pressure data
 *
 *  Created on: Oct 14, 2016
 *      Author: Christian Macias
 */

#ifndef SENSORS_BMP180_PRESSURE_TEMPERATURE_H_
#define SENSORS_BMP180_PRESSURE_TEMPERATURE_H_

extern int bmp180_temperature_data;
extern int bmp180_pressure_data;

/*
 * If  a data type larger in size than unsigned short is added, bmp180_setCoeff must edited to take into account new memory alignment.
 */
typedef struct bmp180_sensor_data_information {
	short AC1;
	short AC2;
	short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	short B1;
	short B2;
	short MB;
	short MC;
	short MD ;
}bmp180_data;

int bmp180_setCoeff(bmp180_data *);
short read_short(char addr);

int bmp180_temperature(bmp180_data *coeff);
int bmp180_raw_temperature(bmp180_data *coeff);
int bmp180_pressure(bmp180_data *coeff);
int bmp180_raw_pressure(bmp180_data *coeff);

#endif /* SENSORS_BMP180_PRESSURE_TEMPERATURE_H_ */
