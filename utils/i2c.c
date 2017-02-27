#include "i2c.h"

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include <driverlib/sysctl.h>
#include "bno055.h"
#include <driverlib/rom.h>

//should put the pressure sensor and the mpu on the same bus. Unnecessary for
//more than 1 i2c bus to be declared.

/*
 * TODO:
 * Determine if BMP180 only operates at 100kbps i2c link
 * or if 400 is alright.
 */

//we want to delay milliseconds
	//SysCtlDelay(1) = 3 clock cycles
	//We have 40MHz
	//1/40Mhz = 2.5e-8
	//SysCtlDelay(1) takes up 7.5e-8 seconds
	//1ms = 1e-3 seconds
	//1e-3 / (7.5e-8) ~ SysCtlDelay(13333)
	 u32 clock;
	 float value;


void initI2C(void) {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3); // Enable I2C3 peripheral
	SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); // Enable GPIOD peripheral
	SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated

    // Use alternate function
    GPIOPinConfigure(GPIO_PD0_I2C3SCL);
    GPIOPinConfigure(GPIO_PD1_I2C3SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0); // Use pin with I2C SCL peripheral
    GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1); // Use pin with I2C SDA peripheral



    I2CMasterInitExpClk(I2C3_BASE, SysCtlClockGet(), false); // Enable and set frequency to 400 kHz
    SysCtlDelay(2); // Insert a few cycles after enabling the I2C to allow the clock to be fully activated

    I2CMasterTimeoutSet(I2C3_BASE, 0xFF);
    SysCtlDelay(2);
    //enable clock stretching for up to 4080 cycles, which is 40.8 ms (unfortunately the max the tiva can take)

    I2CMasterEnable(I2C3_BASE);
    }


//useless
s8 i2cWrite(u8 addr, u8 regAddr, u8 *data, u8 count) {
    i2cWriteData(addr, regAddr, data, 1 );
    return 0;
}

s8 i2cWriteData(u8 addr, u8 regAddr, u8 *data ,u8 length) {

	I2CMasterSlaveAddrSet(I2C3_BASE, addr, false); // Set to write mode

	I2CMasterDataPut(I2C3_BASE, regAddr); // Place address into data register
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Send start condition
	while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done
	I2CMasterDataPut(I2C3_BASE, data[0]);
	delay(5); //wait 5ms just in case;
	I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while(I2CMasterBusy(I2C3_BASE));
	return 0;



}


s8 i2cRead(uint32_t addr, u8 regAddr) {
    I2CMasterSlaveAddrSet(I2C3_BASE, addr, false); // Set to write mode

    I2CMasterDataPut(I2C3_BASE, regAddr); // Place address into data register
    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Send data
    while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done

    I2CMasterSlaveAddrSet(I2C3_BASE, addr, true); // Set to read mode

    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); // Tell master to read data
    while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done
    return I2CMasterDataGet(I2C3_BASE); // Read data
}

s8 i2cReadData(u8 addr, u8 regAddr, u8 *data, u8 length) {
    I2CMasterSlaveAddrSet(I2C3_BASE, addr, false); // Set to write mode

    I2CMasterDataPut(I2C3_BASE, regAddr); // Place address into data register
    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Send data
    while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done

    I2CMasterSlaveAddrSet(I2C3_BASE, addr, true); // Set to read mode

    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // Send start condition
    while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done
    data[0] = I2CMasterDataGet(I2C3_BASE); // Place data into data register

    int i;
    for (i = 1; i < length - 1; i++) {
        I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT); // Send continues condition
        while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done
        data[i] = I2CMasterDataGet(I2C3_BASE); // Place data into data register
    }

    I2CMasterControl(I2C3_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); // Send finish condition
    while (I2CMasterBusy(I2C3_BASE)); // Wait until transfer is done
    if(length != 1)
    {
    data[length - 1] = I2CMasterDataGet(I2C3_BASE); // Place data into data register
    }
    return 0;
}

void delay(u32 ms)
{
	SysCtlDelay((SysCtlClockGet()/(3*1000))*(uint32_t)ms);
}
