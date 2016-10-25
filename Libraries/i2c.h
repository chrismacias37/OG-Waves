/*
 * i2c.h
 *
 *  Created on: Oct 17, 2016
 *      Author: migpa
 */

#ifndef I2C_H_
#define I2C_H_


uint32_t I2CReceive(uint32_t slave_addr, uint8_t reg);

void initI2C(void);

void i2cWrite(uint8_t addr, uint8_t regAddr, uint8_t data);
void i2cWriteData(uint8_t addr, uint8_t regAddr, uint8_t length, uint8_t *data );

//this function is for reading one byte of data only
uint8_t i2cRead(uint8_t addr, uint8_t regAddr);

//this function reads multiple bytes of data at a time
void i2cReadData(uint8_t addr, uint8_t regAddr, uint8_t length, uint8_t *data );




#endif /* I2C_H_ */
