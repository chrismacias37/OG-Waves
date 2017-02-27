/*
 * i2c.h
 *
 *  Created on: Oct 17, 2016
 *      Author: migpa
 */

#ifndef UTILS_I2C_H_
#define UTILS_I2C_H_
#include <stdint.h>
#include <stdbool.h>
#include "bno055.h"

u32 clock;
float value;

void initI2C(void);

s8 i2cWrite(u8 addr, u8 regAddr, u8 *data, u8 count);
s8 i2cWriteData(u8 addr, u8 regAddr, u8 *data, u8 length);

//this function is for reading one byte of data only
s8 i2cRead(uint32_t addr, u8 regAddr);

//this function reads multiple bytes of data at a time
s8 i2cReadData(u8 addr, u8 regAddr, u8 *data , u8 length );
void delay(u32 ms);



#endif /* UTILS_I2C_H_ */
