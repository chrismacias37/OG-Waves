/*
 * i2c.c
 *
 *  Created on: Oct 4, 2016
 *      Author: ChristianMacias
 */
#include<driverlib/i2c.h>


int initalize_i2c()
{
	I2CMasterInitExpClk(0x4020000,SysCtlClockGet(),1); //Set up I2C0 Module to master mode and set it to 400kbps
	return 1;
}
