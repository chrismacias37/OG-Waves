/*
 * bmp180_pressure_temperature.c
 *	Description:
 *	   Retrieve and transmit data to the Bosh BMP180 temperature/pressure data
 *
 *  Created on: Oct 14, 2016
 *      Author: Christian Macias, Marc Villareal
 */
#include <stdbool.h>
#include <stdint.h>


#include <driverlib/i2c.h>
#include <driverlib/sysctl.h>

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
	static int boot_wait = 0;
	if(boot_wait == 1)
	{
		SysCtlDelay(SysCtlClockGet() * 2);// 5sec+ Delay to fix incorrect values at boot
		boot_wait = 1;
	}


	char bmp_reg = 0xAA;//register to read starting from 0xAA
	unsigned short *ptr =(unsigned short *) &(data->AC1);
	while(bmp_reg<0xBF)
	{
		*ptr=0;//Clear data
		*ptr=read_short(bmp_reg);
		ptr= ptr + 1; //move to next element in bmp180_data, assuming consecutive alignment
		bmp_reg += 2;
	}
	return 0; //Dummy return. Need to implement error checking
}

short read_short(char addr)
{
	/*
	 * Read two bytes of data starting from addr.
	 */
	short value=0;

	I2CMasterSlaveAddrSet(I2C_MODULE, BMP180_ADDR, 0);
	I2CMasterDataPut(I2C_MODULE, addr);//register to read
	I2CMasterControl(I2C_MODULE ,I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(I2C_MODULE));

	I2CMasterSlaveAddrSet(I2C_MODULE, BMP180_ADDR, 1);
	I2CMasterControl(I2C_MODULE ,I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C_MODULE));

	value=I2CMasterDataGet(I2C_MODULE);//Grab MSB(Most significant byte)
	value = value << 8; //shift 8 bit to make room for next byte

	I2CMasterSlaveAddrSet(I2C_MODULE, BMP180_ADDR, 0);
	I2CMasterDataPut(I2C_MODULE, addr + 1);//register to read + 1 to read next byte
	I2CMasterControl(I2C_MODULE ,I2C_MASTER_CMD_SINGLE_SEND);
	while(I2CMasterBusy(I2C_MODULE));

	I2CMasterSlaveAddrSet(I2C_MODULE, BMP180_ADDR, 1);
	I2CMasterControl(I2C_MODULE ,I2C_MASTER_CMD_SINGLE_RECEIVE);
	while(I2CMasterBusy(I2C_MODULE));
	value |= (I2CMasterDataGet(I2C_MODULE));//Grab LSB(Least significant byte)

	return value;
}

long bmp180_temperature(bmp180_data *coeff)
{


	int UT = 0;
	int X1,X2,B5,T;
	UT = bmp180_raw_temperature(coeff);
	X1 = (UT - coeff->AC6)*coeff->AC5/32768;
	X2 = coeff->MC*2048 / (X1 + coeff->MD);
	B5 = X1 + X2;
	T = (B5 + 8)/16;
	return T;
}

long bmp180_raw_temperature(bmp180_data *coeff)
{
	long UT = 0;

	I2CMasterSlaveAddrSet(I2C_MODULE, BMP180_ADDR, 0);
	I2CMasterDataPut(I2C_MODULE, 0xF4);//Control register
	I2CMasterControl(I2C_MODULE , I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C_MODULE));

	I2CMasterDataPut(I2C_MODULE, 0x2E);//Control register
	I2CMasterControl(I2C_MODULE , I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C_MODULE));

	//Must wait at least 4.5ms before reading UT data
	SysCtlDelay(SysCtlClockGet());//Delays 1-3 seconds.

	UT=(int)read_short(0xF6);


	return UT;
}

long bmp180_pressure(bmp180_data *coeff)
{
	/*
	 * Stub function. Must convert raw pressure data (UP) to hPa
	 */
	long UT = 0;
	long X1,X2,B5;
	UT = bmp180_raw_temperature(coeff);
	X1 = (UT - coeff->AC6)*coeff->AC5/32768;
	X2 = coeff->MC * 2048 / (X1 + coeff->MD);
	B5 = X1 + X2;

	long UP = 0;
	long  B6,X3,p, B3;
	unsigned long B4, B7;
	UP = bmp180_raw_pressure(coeff);
	B6 = B5 - 4000;
	X1 = (coeff->B2*(B6*B6/4096))/2048;
	X2 = coeff->AC2 * B6/2048;
	X3 = X1 + X2;
	B3 = (((coeff->AC1*4+X3)<< 0)+2)/4;
	X1 = coeff->AC3 * B6/8192;
	X2 = (coeff->B1*(B6*B6/4096))/65536;
	X3 = ((X1 + X2)+2)/4;
	B4 = coeff->AC4*(unsigned long)(X3+32768)/32768;
	B7 = ((unsigned long)UP - B3)*(50000>>0);
	if (B7<0x80000000)
	{
		p = (B7*2)/B4;
	}
	else
	{
		p = (B7/B4)*2;
	}

	X1 = (p/256)*(p/256);
	X1 = (X1 * 3038)/65536;
	X2 = (-7357 * p)/65536;
	p = p + (X1 + X2 + 3791)/16;


	return p;
}

long bmp180_raw_pressure(bmp180_data *coeff)
{
	//assumes OSS = 0

	long UP = 0;

	I2CMasterSlaveAddrSet(I2C_MODULE, BMP180_ADDR, 0);
	I2CMasterDataPut(I2C_MODULE, 0xF4);//Control register
	I2CMasterControl(I2C_MODULE , I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C_MODULE));

	I2CMasterDataPut(I2C_MODULE, 0x34);//Control register
	I2CMasterControl(I2C_MODULE , I2C_MASTER_CMD_BURST_SEND_FINISH);
	while(I2CMasterBusy(I2C_MODULE));

	//Must wait at least 4.5ms before reading UT data
	SysCtlDelay(SysCtlClockGet());//Delays 1-3 seconds.

	UP=(int)read_short(0xF6);


	return UP;
}
