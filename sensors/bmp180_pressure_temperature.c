/*
 * bmp180_pressure_temperature.c
 *	Description:
 *	   Retrieve and transmit data to the Bosh BMP180 temperature/pressure data
 *
 *  Created on: Oct 14, 2016
 *      Author: Christian Macias
 */
#include <stdbool.h>
#include <stdint.h>


#include <driverlib/i2c.h>

#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include "bmp180_pressure_temperature.h"

int bmp180_setCoeff(bmp180_data *data)
{
	/*
	 * This function assumes the memory alignment in data is consecutive with no padding
	 * except at the end of the structure.
	 *
	 * BUG: sets incorrect values if called more than once.
	 */
	char bmp_reg = 0xAA;//register to read starting from 0xAA
	unsigned short *ptr = &(data->AC1);
	while(bmp_reg<0xBF)
	{
		*ptr=0;//Clear data
		read_short(ptr,bmp_reg);
		ptr= ptr + 1; //move to next element in array, assuming consecutive alignment
		bmp_reg += 2;
	}
	return 0; //Dummy return. Need to implement error checking
}

int read_short(unsigned short *ptr, char addr)
{
	I2CMasterSlaveAddrSet(I2C0_BASE, 0b1110111, 0);
	I2CMasterDataPut(I2C0_BASE, addr);//register to read
	I2CMasterControl(I2C0_BASE ,I2C_MASTER_CMD_SINGLE_SEND);

	I2CMasterSlaveAddrSet(I2C0_BASE, 0b1110111, 1);
	I2CMasterControl(I2C0_BASE ,I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C0_BASE));
	*ptr=I2CMasterDataGet(I2C0_BASE);//Grab MSB(Most significant byte)
	*ptr= (*ptr) << 8; //shift 8 bit to make room for next byte

	I2CMasterSlaveAddrSet(I2C0_BASE, 0b1110111, 0);
	I2CMasterDataPut(I2C0_BASE, addr + 1);//register to read + 1 to read next byte
	I2CMasterControl(I2C0_BASE ,I2C_MASTER_CMD_SINGLE_SEND);

	I2CMasterSlaveAddrSet(I2C0_BASE, 0b1110111, 1);
	I2CMasterControl(I2C0_BASE ,I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C0_BASE));
	*ptr |= (I2CMasterDataGet(I2C0_BASE));//Grab LSB(Least significant byte)

	return 1;//Dummy return
}

int bmp180_temperature(bmp180_data *coeff)
{
	/*
	 * Stub function. Must convert raw temperature data (UT) to fahrenheit
	 */

	int UT = 0;
	UT = bmp180_raw_temperature(coeff);

	//............//
}

int bmp180_raw_temperature(bmp180_data *coeff)
{

	/*
	 * Stub function. Must read raw temperature data
	 */

	return 0;
}

int bmp180_pressure(bmp180_data *coeff)
{
	/*
	 * Stub function. Must convert raw pressure data (UP) to hPa
	 */

	int UP = 0;
	UP = bmp180_raw_pressure(coeff);

	//............//
}

int bmp180_raw_pressure(bmp180_data *coeff)
{

	/*
	 * Stub function. Must read raw pressure
	 */

	return 0;
}
