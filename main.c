/*
 * main.c
 */
#include<driverlib/sysctl.h>
#include"hardware/gpio.h"

int main(void) {

	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);//Set the proccessor speed to 5Mhz
	I2CMasterInitExpClk(I2C0_BASE,SysCtlClockGet(),1); //Set up I2C0 Module to master mode and set it to 400kbps


}
