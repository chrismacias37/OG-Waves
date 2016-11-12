/*
 * accgyro.h
 *
 *  Created on: Oct 27, 2016
 *      Author: migpa
 */

#ifndef ACCGYRO_H_
#define ACCGYRO_H_

void initMPU9250();
void calibrateMPU9250();
void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz);;
void initMagnet(float *destination);
void readAccel(int16_t *destination);
void readGyro(int16_t *destination);
void readMagnet(int16_t *destination);


#endif /* ACCGYRO_H_ */
