/*
 * UTEP Cubesat 2016-17 Senior Project
 *
 * Authors: Christian Macias,
 */
#include <stdbool.h>
#include <stdint.h>

#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

#include <driverlib/sysctl.h>
#include <driverlib/i2c.h>
#include <driverlib/rom.h>
#include <driverlib/gpio.h>


int main(void) {

	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	//Sets up the pins to the correct pinmux
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE,0b00000100);
	GPIOPinTypeI2C(GPIO_PORTB_BASE,0b00001000);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	I2CMasterInitExpClk(I2C0_BASE,SysCtlClockGet(),0); //Set up I2C0 Module to master mode and set it to 400kbps

	uint32_t data = 0;

	while(1)
	{
		I2CMasterSlaveAddrSet(I2C0_BASE, 0b1110111, 0); //Set salve address and set to transmit mode
		I2CMasterDataPut(I2C0_BASE, 0x2E);
		I2CMasterControl(I2C0_BASE ,I2C_MASTER_CMD_SINGLE_SEND);

		I2CMasterSlaveAddrSet(I2C0_BASE, 0b1110111, 1); //Set salve address and set to Receive mode
		I2CMasterDataPut(I2C0_BASE, 0x2E);
		I2CMasterControl(I2C0_BASE ,I2C_MASTER_CMD_SINGLE_RECEIVE);

		data=I2CMasterDataGet(I2C0_BASE);

	}


}
