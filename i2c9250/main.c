#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/i2c.h"
#include "i2c.h"
#include "uart.h"
#include "uartstdio.h"
#include "driverlib/fpu.h"
#include "accgyro.h"
#include "conversions.h"
#define MPU6500_WHO_AM_I	0x75

#define MPU6500_ADDRESS		0x68
#define MPU6500_WHO_AM_I_ID	0x73

#define MPU6500_USER_CONTROL 106
#define MPU6500_PWR_MGMT1	107
#define MPU6500_PWR_MGMT2	108
#define MPU6500_SMPLRT_DIV 25


//sensitivity is 208.776 m/s per LSB

float accelSensitivity = 208.776;
float gyroSensitivity = 16.4;
float magSensitivity = 0.6; //this factor will be multiplied to get microteslas!!!!!!!!!!
char accelSens[50];
int16_t accel[3];
int16_t gyro[3];
int16_t mag[3];
int32_t accelOffset[3];
int32_t gyroOffset[3];
float magOffset[3] = {0.0, 0.0, 0.0};
extern float q[4];
uint16_t teapotData[8]; //to convert floating point to integer
float temp[4]; //to store decimal points
float a[3];
float g[3];
float m[3];
char axstring[50];
char aystring[50];
char azstring[50];

char gxstring[50];
char gystring[50];
char gzstring[50];

char mxstring[50];
char mystring[50];
char mzstring[50];

char q1string[50];
char q2string[50];
char q3string[50];
char q4string[50];

float magmag;


int main(void) {
	
	//set the clock to 40 MHz
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);


	//initialize the I2C module

	initI2C(); //enables I2C on ports PB2 (SCL) and PB3 (SDA)
	ConfigureUART();
	//UARTprintf("Hello World!\n");
	FPUEnable(); //turn on floating point calculations
	initMPU9250();
	//we have the offsets available in the main program, use them!
	calibrateMPU9250(gyroOffset, accelOffset);
	initMagnet(magOffset);
	//fully aware of single precision calculations

	readAccel(accel);



	//UARTprintf("gyro x offset %d", gyroOffset[0]);
	float2String(accelSensitivity, accelSens);
	UARTprintf("Accelerometer Sensitivity: %s", accelSens);
	SysCtlDelay(10000000);
	while(1)
	{


		//returns uncalibrated data
		readAccel(accel);
		readGyro(gyro);
		readMagnet(mag);
		a[0] = ((float)(accel[0] - accelOffset[0]))/accelSensitivity;
		a[1] = ((float)(accel[1] - accelOffset[1]))/accelSensitivity;
		a[2] = ((float)(accel[2]))/accelSensitivity;
		float2String(a[0], axstring);
		float2String(a[1], aystring);
		float2String(a[2], azstring);


		g[0] = ((float)(gyro[0] - gyroOffset[0]))/gyroSensitivity;
		g[1] = ((float)(gyro[1] - gyroOffset[1]))/gyroSensitivity;
		g[2] = ((float)(gyro[2] - gyroOffset[2]))/gyroSensitivity;
		float2String(g[0], gxstring);
		float2String(g[1], gystring);
		float2String(g[2], gzstring);


		m[0] = ((float)(mag[0] * magOffset[0]))*magSensitivity;
		m[1] = ((float)(mag[1] * magOffset[1]))*magSensitivity;
		m[2] = ((float)(mag[2] * magOffset[2]))*magSensitivity;


		float2String(m[0], mxstring);
		float2String(m[1], mystring);
		float2String(m[2], mzstring);
		//float2String()


		//UARTprintf("Acceleration in x, y, z direction in m/s^2: %s, %s, %s\n", axstring, aystring, azstring);

		//UARTprintf("Rotation in x, y, z direction in m/s^2: %s, %s, %s\n", gxstring, gystring, gzstring);

		//UARTprintf("B field in x, y, z direction in m/s^2: %d, %d, %d\n", m[0], m[1], m[2]);


		SysCtlDelay(1000000);

		/*
		 * The quaternion function requires floats to work with.
		 * Implement floats as output of the read functions.
		 */
		MadgwickQuaternionUpdate(a[0], a[1], a[2], g[0], g[1], g[2], m[0], m[1], m[2]);

		float2String(q[0], q1string);
		float2String(q[1], q2string);
		float2String(q[2], q3string);
		float2String(q[3], q4string);
		//UARTprintf("Quaternion: {%s %s %s %s}\n", q1string,q2string,q3string,q4string);
		SysCtlDelay(1000000);




		/*
		for(i = 0; i < 4; i +=2)
		{
			teapotData[i] = (int)q[i];
			temp[i] = q[i] - (float)teapotData[i];
			teapotData[i+1] = temp[i]*100;
			UARTprintf("%d.%d"teapotData[i], teapotData[i+1]);

		}
		*/

		/*
		 * TODO:
		 * Output values as floats for more accuracy - 9.8 m/s^2 rather than 9
		 * Implement Mahony Fusion Filter
		 * Implement function to read both accelerometer and gyroscope at once
		 * Magnetometer work
		 * Interrupts so that we won't be polling, we will pull the value whenever needed
		 * Store this data and compress it
		 * Implement a debug #if
		 * Implement a nice demo to demonstrate the ability of the Mahony Filter to show absolute orientation
		 * Research low power mode on accelerometer
		 * Store zero values of gyroscope in microcontroller long term memory somehow
		 * Store offsets in the offset registers of the MPU 9250 to reduce number of computations on the TIVA
		 * Research quaternions and how they work
		 * I2C Master Module on this device
		 * Combine Pressure and Temp sensor with this device.
		 *
		 */
	}

	return 0;
}
