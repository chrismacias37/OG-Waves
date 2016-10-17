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
	unsigned short AC1;
	unsigned short AC2;
	unsigned short AC3;
	unsigned short AC4;
	unsigned short AC5;
	unsigned short AC6;
	unsigned short B1;
	unsigned short B2;
	unsigned short MB;
	unsigned short MC;
	unsigned short MD ;
}bmp180_data;

int bmp180_setCoeff(bmp180_data *);
int read_short(unsigned short *ptr, char addr);

int bmp180_temperature(bmp180_data *coeff);
int bmp180_raw_temperature(bmp180_data *coeff);
int bmp180_pressure(bmp180_data *coeff);
int bmp180_raw_pressure(bmp180_data *coeff);

#endif /* SENSORS_BMP180_PRESSURE_TEMPERATURE_H_ */
