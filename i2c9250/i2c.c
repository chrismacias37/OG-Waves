#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"

void initI2C(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0); // Enable I2C0 peripheral
    SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB); // Enable GPIOB peripheral
    SysCtlDelay(2); // Insert a few cycles after enabling the peripheral to allow the clock to be fully activated

    // Use alternate function
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2); // Use pin with I2C SCL peripheral
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3); // Use pin with I2C peripheral

    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true); // Enable and set frequency to 400 kHz

    SysCtlDelay(2); // Insert a few cycles after enabling the I2C to allow the clock to be fully activated
}


//useless
void i2cWrite(uint8_t addr, uint8_t regAddr, uint8_t data) {
    i2cWriteData(addr, regAddr, 1, &data);
}

void i2cWriteData(unsigned char addr, unsigned char regAddr, unsigned char length, unsigned char *data) {
    I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // Set to write mode

    I2CMasterDataPut(I2C0_BASE, regAddr); // Place address into data register
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); // Send start condition
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done

    uint8_t i;
    for (i = 0; i < length - 1; i++) {
        I2CMasterDataPut(I2C0_BASE, data[i]); // Place data into data register
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_CONT); // Send continues condition
        while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
    }

    I2CMasterDataPut(I2C0_BASE, data[length - 1]); // Place data into data register
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); // Send finish condition
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
}

uint8_t i2cRead(uint8_t addr, uint8_t regAddr) {
    I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // Set to write mode

    I2CMasterDataPut(I2C0_BASE, regAddr); // Place address into data register
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND); // Send data
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done

    I2CMasterSlaveAddrSet(I2C0_BASE, addr, true); // Set to read mode

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE); // Tell master to read data
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
    return I2CMasterDataGet(I2C0_BASE); // Read data
}

void i2cReadData(unsigned char addr, unsigned char regAddr, unsigned char length, unsigned char *data) {
    I2CMasterSlaveAddrSet(I2C0_BASE, addr, false); // Set to write mode

    I2CMasterDataPut(I2C0_BASE, regAddr); // Place address into data register
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND); // Send data
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done

    I2CMasterSlaveAddrSet(I2C0_BASE, addr, true); // Set to read mode

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START); // Send start condition
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
    data[0] = I2CMasterDataGet(I2C0_BASE); // Place data into data register

    uint8_t i;
    for (i = 1; i < length - 1; i++) {
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_CONT); // Send continues condition
        while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
        data[i] = I2CMasterDataGet(I2C0_BASE); // Place data into data register
    }

    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH); // Send finish condition
    while (I2CMasterBusy(I2C0_BASE)); // Wait until transfer is done
    data[length - 1] = I2CMasterDataGet(I2C0_BASE); // Place data into data register
}
