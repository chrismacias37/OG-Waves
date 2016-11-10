/*
 * UTEP Cubesat 2016-17 Senior Project
 *
 * Authors: Christian Macias,
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

#include <driverlib/sysctl.h>
#include <driverlib/rom.h>
#include <driverlib/i2c.h>
#include <driverlib/gpio.h>

#include"sensors/bmp180_pressure_temperature.h"
#include"sensors/sdCardAdapter.h"

int main(void){

	bmp180_data bmp180;//Holds bmp180's coeff data

	//Global variables to be used in interrupts
	volatile int bmp180_temperature_data = 0;
	volatile long bmp180_pressure_data = 0;

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);//i2c
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);//ssi

	//Sets up the pins to the correct pinmux
	GPIOPinTypeI2C(GPIO_PORTB_BASE, 0b00001000);
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, 0b00000100);


	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	I2CMasterInitExpClk(I2C0_BASE,SysCtlClockGet(), false); //Set up I2C0 Module to master mode and set it to 100kbps



	bmp180_setCoeff(&bmp180);//Set the bmp180 coefficients
	bmp180_temperature_data = bmp180_temperature(&bmp180);
	bmp180_pressure_data = bmp180_pressure(&bmp180)-44402; //-44402 is to try and compensate

	while(1)
	{
		bmp180_pressure_data = bmp180_pressure(&bmp180)-44402;
		bmp180_temperature_data = bmp180_temperature(&bmp180);
	}
}
