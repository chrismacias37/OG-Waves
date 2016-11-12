#include "i2c.h"
#include "driverlib/sysctl.h"
#include "accgyro.h"
#include "driverlib/fpu.h"
#include <math.h>
#include "uartstdio.h"
#include "conversions.h"
#define MPU9250_ADDRESS 0x68

// See also MPU-9250 Register Map and Descriptions, Revision 4.0, RM-MPU-9250A-00, Rev. 1.4, 9/9/2013 for registers not listed in
// above document; the MPU9250 and MPU9150 are virtually identical but the latter has a different register map
//
//Magnetometer Registers
#define AK8963_ADDRESS   0x0C<<1
#define WHO_AM_I_AK8963  0x00 // should return 0x48
#define INFO             0x01
#define AK8963_ST1       0x02  // data ready status bit 0
#define AK8963_XOUT_L    0x03  // data
#define AK8963_XOUT_H    0x04
#define AK8963_YOUT_L    0x05
#define AK8963_YOUT_H    0x06
#define AK8963_ZOUT_L    0x07
#define AK8963_ZOUT_H    0x08
#define AK8963_ST2       0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8963_CNTL      0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8963_ASTC      0x0C  // Self test control
#define AK8963_I2CDIS    0x0F  // I2C disable
#define AK8963_ASAX      0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8963_ASAY      0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8963_ASAZ      0x12  // Fuse ROM z-axis sensitivity adjustment value

#define SELF_TEST_X_GYRO 0x00
#define SELF_TEST_Y_GYRO 0x01
#define SELF_TEST_Z_GYRO 0x02

/*#define X_FINE_GAIN      0x03 // [7:0] fine gain
#define Y_FINE_GAIN      0x04
#define Z_FINE_GAIN      0x05
#define XA_OFFSET_H      0x06 // User-defined trim values for accelerometer
#define XA_OFFSET_L_TC   0x07
#define YA_OFFSET_H      0x08
#define YA_OFFSET_L_TC   0x09
#define ZA_OFFSET_H      0x0A
#define ZA_OFFSET_L_TC   0x0B */

#define SELF_TEST_X_ACCEL 0x0D
#define SELF_TEST_Y_ACCEL 0x0E
#define SELF_TEST_Z_ACCEL 0x0F

#define SELF_TEST_A      0x10

#define XG_OFFSET_H      0x13  // User-defined trim values for gyroscope
#define XG_OFFSET_L      0x14
#define YG_OFFSET_H      0x15
#define YG_OFFSET_L      0x16
#define ZG_OFFSET_H      0x17
#define ZG_OFFSET_L      0x18
#define SMPLRT_DIV       0x19
#define CONFIG           0x1A
#define GYRO_CONFIG      0x1B
#define ACCEL_CONFIG     0x1C
#define ACCEL_CONFIG2    0x1D
#define LP_ACCEL_ODR     0x1E
#define WOM_THR          0x1F

#define MOT_DUR          0x20  // Duration counter threshold for motion interrupt generation, 1 kHz rate, LSB = 1 ms
#define ZMOT_THR         0x21  // Zero-motion detection threshold bits [7:0]
#define ZRMOT_DUR        0x22  // Duration counter threshold for zero motion interrupt generation, 16 Hz rate, LSB = 64 ms

#define FIFO_EN          0x23
#define I2C_MST_CTRL     0x24
#define I2C_SLV0_ADDR    0x25
#define I2C_SLV0_REG     0x26
#define I2C_SLV0_CTRL    0x27
#define I2C_SLV1_ADDR    0x28
#define I2C_SLV1_REG     0x29
#define I2C_SLV1_CTRL    0x2A
#define I2C_SLV2_ADDR    0x2B
#define I2C_SLV2_REG     0x2C
#define I2C_SLV2_CTRL    0x2D
#define I2C_SLV3_ADDR    0x2E
#define I2C_SLV3_REG     0x2F
#define I2C_SLV3_CTRL    0x30
#define I2C_SLV4_ADDR    0x31
#define I2C_SLV4_REG     0x32
#define I2C_SLV4_DO      0x33
#define I2C_SLV4_CTRL    0x34
#define I2C_SLV4_DI      0x35
#define I2C_MST_STATUS   0x36
#define INT_PIN_CFG      0x37
#define INT_ENABLE       0x38
#define DMP_INT_STATUS   0x39  // Check DMP interrupt
#define INT_STATUS       0x3A
#define ACCEL_XOUT_H     0x3B
#define ACCEL_XOUT_L     0x3C
#define ACCEL_YOUT_H     0x3D
#define ACCEL_YOUT_L     0x3E
#define ACCEL_ZOUT_H     0x3F
#define ACCEL_ZOUT_L     0x40
#define TEMP_OUT_H       0x41
#define TEMP_OUT_L       0x42
#define GYRO_XOUT_H      0x43
#define GYRO_XOUT_L      0x44
#define GYRO_YOUT_H      0x45
#define GYRO_YOUT_L      0x46
#define GYRO_ZOUT_H      0x47
#define GYRO_ZOUT_L      0x48
#define EXT_SENS_DATA_00 0x49
#define EXT_SENS_DATA_01 0x4A
#define EXT_SENS_DATA_02 0x4B
#define EXT_SENS_DATA_03 0x4C
#define EXT_SENS_DATA_04 0x4D
#define EXT_SENS_DATA_05 0x4E
#define EXT_SENS_DATA_06 0x4F
#define EXT_SENS_DATA_07 0x50
#define EXT_SENS_DATA_08 0x51
#define EXT_SENS_DATA_09 0x52
#define EXT_SENS_DATA_10 0x53
#define EXT_SENS_DATA_11 0x54
#define EXT_SENS_DATA_12 0x55
#define EXT_SENS_DATA_13 0x56
#define EXT_SENS_DATA_14 0x57
#define EXT_SENS_DATA_15 0x58
#define EXT_SENS_DATA_16 0x59
#define EXT_SENS_DATA_17 0x5A
#define EXT_SENS_DATA_18 0x5B
#define EXT_SENS_DATA_19 0x5C
#define EXT_SENS_DATA_20 0x5D
#define EXT_SENS_DATA_21 0x5E
#define EXT_SENS_DATA_22 0x5F
#define EXT_SENS_DATA_23 0x60
#define MOT_DETECT_STATUS 0x61
#define I2C_SLV0_DO      0x63
#define I2C_SLV1_DO      0x64
#define I2C_SLV2_DO      0x65
#define I2C_SLV3_DO      0x66
#define I2C_MST_DELAY_CTRL 0x67
#define SIGNAL_PATH_RESET  0x68
#define MOT_DETECT_CTRL  0x69
#define USER_CTRL        0x6A  // Bit 7 enable DMP, bit 3 reset DMP
#define PWR_MGMT_1       0x6B // Device defaults to the SLEEP mode
#define PWR_MGMT_2       0x6C
#define DMP_BANK         0x6D  // Activates a specific bank in the DMP
#define DMP_RW_PNT       0x6E  // Set read/write pointer to a specific start address in specified DMP bank
#define DMP_REG          0x6F  // Register in DMP from which to read or to which to write
#define DMP_REG_1        0x70
#define DMP_REG_2        0x71
#define FIFO_COUNTH      0x72
#define FIFO_COUNTL      0x73
#define FIFO_R_W         0x74
#define WHO_AM_I_MPU9250 0x75 // Should return 0x73
#define XA_OFFSET_H      0x77
#define XA_OFFSET_L      0x78
#define YA_OFFSET_H      0x7A
#define YA_OFFSET_L      0x7B
#define ZA_OFFSET_H      0x7D
#define ZA_OFFSET_L      0x7E


float PI = 3.14159265358979323846;
float GyroMeasError = 3.14159265358979323846 * (60.0 / 180.0);     // gyroscope measurement error in rads/s (start at 60 deg/s), then reduce after ~10 s to 3
float beta = 0.86602540378 * 3.14159265358979323846 * (60.0 / 180.0);  // compute beta
float GyroMeasDrift = 3.14159265358979323846 * (1.0 / 180.0);      // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
float zeta = 0.86602540378* 3.14159265358979323846 * (60.0 / 180.0);  // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value

float pitch, yaw, roll;
float deltat = 0.0f;                             // integration interval for both filter schemes
int lastUpdate = 0, firstUpdate = 0, Now = 0;    // used to calculate integration interval                               // used to calculate integration interval
float q[4] = {1.0f, 0.0f, 0.0f, 0.0f};           // vector to hold quaternion
float eInt[3] = {0.0f, 0.0f, 0.0f};              // vector to hold integral error for Mahony method




char d[50] = {' '};


void delay(int millis)
{
	uint16_t time = SysCtlClockGet() /(1000*3);
	uint16_t delay = millis * time;
	SysCtlDelay(delay);
}

void initMPU9250()
{
 // Initialize MPU9250 device
 // wake up device
  i2cWrite(MPU9250_ADDRESS, PWR_MGMT_1, 0x80); // Clear sleep mode bit (6), enable all sensors

  //define a wait function for milliseconds
  SysCtlDelay(100000); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt

 // get stable time source
  i2cWrite(MPU9250_ADDRESS, PWR_MGMT_1, (1<<3) | (1<<0));  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001

 // Configure Gyro and Accelerometer
 // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively;
 // DLPF_CFG = bits 2:0 = 010; this sets the sample rate at 1 kHz for both
 // Maximum delay is 4.9 ms which is just over a 200 Hz maximum rate
  // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
  i2cWrite(MPU9250_ADDRESS, SMPLRT_DIV, 0x00);  // Use a 1khz Hz rate; the same rate set in CONFIG above

  i2cWrite(MPU9250_ADDRESS, CONFIG, 0x03);


 // Set gyroscope full scale range
 // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
  //fix later

    i2cWrite(MPU9250_ADDRESS, GYRO_CONFIG, 0x18); // Set 2000 dps full scale range for the gyro

 // Set accelerometer configuration

  //fix this
  i2cWrite(MPU9250_ADDRESS,ACCEL_CONFIG ,0x18);
 // Set accelerometer sample rate configuration
 // It is possible to get a 4 kHz sample rate from the accelerometer by choosing 1 for
 // accel_fchoice_b bit [3]; in this case the bandwidth is 1.13 kHz
  i2cWrite(MPU9250_ADDRESS, ACCEL_CONFIG2, 0x03); // Set accelerometer rate to 1 kHz and bandwidth to 41 Hz

 // The accelerometer, gyro, and thermometer are set to 1 kHz sample rates,
 // but all these rates are further reduced by a factor of 5 to 200 Hz because of the SMPLRT_DIV setting

  // Configure Interrupts and Bypass Enable
  // Set interrupt pin active high, push-pull, and clear on read of INT_STATUS, enable I2C_BYPASS_EN so additional chips
  // can join the I2C bus and all can be controlled by the Arduino as master
   i2cWrite(MPU9250_ADDRESS, INT_PIN_CFG, (1<<5)|(1<<4)|(1<<1));
   i2cWrite(MPU9250_ADDRESS, INT_ENABLE, (1<<0));  // Enable data ready (bit 0) interrupt
}


void calibrateMPU9250(int32_t *dest1, int32_t *dest2)
{

	uint8_t data[12]; //data to be read from accelerometer and gyro data
	int32_t gyro_bias[3] = {0,0,0}, accel_bias[3] = {0,0,0};



	//calibrate the sensor under normal operating conditions.


	uint16_t i = 0;

	for(i=0; i<250 ;i++)
	{
		int16_t accel_temp[3] = {0,0,0}, gyro_temp[3] = {0, 0, 0};
		i2cReadData(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, data);
		accel_temp[0] = (int16_t) (((int16_t)data[0]<<8 | data[1]));
		accel_temp[1] = (int16_t) (((int16_t)data[2]<<8 | data[3]));
		accel_temp[2] = (int16_t) (((int16_t)data[4]<<8 | data[5]));
		UARTprintf("Accelx, y, z: %d, %d, %d \n", accel_temp[0], accel_temp[1], accel_temp[2]);
		i2cReadData(MPU9250_ADDRESS, GYRO_XOUT_H, 6, data);
		gyro_temp[0] = (int16_t) (((int16_t)data[0]<<8 | data[1]));
		gyro_temp[1] = (int16_t) (((int16_t)data[2]<<8 | data[3]));
		gyro_temp[2] = (int16_t) (((int16_t)data[4]<<8 | data[5]));
		UARTprintf("Gyrox, y, z: %d, %d, %d \n", gyro_temp[0], gyro_temp[1], gyro_temp[2] );



		accel_bias[0] =  accel_bias[0] + (int32_t)accel_temp[0];
		accel_bias[1] =  accel_bias[1] +(int32_t)accel_temp[1];
		accel_bias[2] = accel_bias[2] + (int32_t)accel_temp[2];
		gyro_bias[0] = gyro_bias[0] + (int32_t)gyro_temp[0];
		gyro_bias[1] = gyro_bias[1] + (int32_t)gyro_temp[1];
		gyro_bias[2] = gyro_bias[2] + (int32_t)gyro_temp[2];

	}






	UARTprintf("gyro bias %d" ,gyro_bias[0] / i);
	SysCtlDelay(100000);
	accel_bias[0] = (accel_bias[0] / 250);
	accel_bias[1] = (accel_bias[1] / 250);
	accel_bias[2] = (accel_bias[2] / 250);
	gyro_bias[0] = (gyro_bias[0] / 250);
	gyro_bias[1] = (gyro_bias[1] / 250);
	gyro_bias[2] = (gyro_bias[2] / 250);


	dest1[0] = (int32_t)gyro_bias[0]; //gyro bias in native gyro units
	dest1[1] = (int32_t)gyro_bias[1]; //gyro bias in native units
	dest1[2] = (int32_t)gyro_bias[2]; //gyro bias in deg/s


	dest2[0] = (int32_t)accel_bias[0];
	dest2[1] = (int32_t)accel_bias[1];
	dest2[2] = (int32_t)accel_bias[2];

	UARTprintf("gyrox offset: %d\n", dest1[0]);
	UARTprintf("accelx offset: %d\n", dest2[0]);

}



void initMagnet(float *destination)
{
	uint8_t data[3];
	//write code to initialize the magnetometer
	i2cRead(AK8963_ADDRESS, )
	//lets check if we are talking to the right thing
	i2cWrite(AK8963_ADDRESS, AK8963_CNTL, 0x00); //soft reset
	SysCtlDelay(10000);
	i2cWrite(AK8963_ADDRESS, AK8963_CNTL, 0x0F);
	SysCtlDelay(10000);
	i2cReadData(AK8963_ADDRESS, AK8963_ASAX, 3, &data[0]); //read calibration values
	UARTprintf("x calibration value = %d, %d, %d\n", data[0], data[1], data[2]);
	SysCtlDelay(1000000);
	destination[0] = (float)((data[0] - 128)/256.0) + 1.0;//(float)(data[0] - 128)/256.0 + 1.0;
	destination[1] = (float)((data[1] - 128)/256.0) + 1.0;; //(float)(data[1] - 128)/256.0 + 1.0;
	destination[2] = (float)((data[2] - 128)/256.0) + 1.0;;//(float)(data[2] - 128)/256.0 + 1.0;
	float2String(destination[0], d);
	UARTprintf("Mag x offset: %s\n", d);
	SysCtlDelay(10000);
	SysCtlDelay(10000);
	//configure magnetometer for reading at highest res;
	SysCtlDelay(10000);

}

void readMagnet(int16_t *destination)
{
	uint8_t data[7]; //magdata + st2 register
	UARTprintf("You are here!\n");
	if(i2cRead(AK8963_ADDRESS, AK8963_ST1) & 0x01)
	{
		UARTprintf("Data is ready\n");
		SysCtlDelay(1000000);
		i2cReadData(AK8963_ADDRESS, AK8963_XOUT_L, 7, &data[0]);
		uint8_t c = data[6];
		if(!(c & 0x08)) //check if data has overflowed
		{
			UARTprintf("No overflow\n");
			SysCtlDelay(1000000);
			destination[0] = (int16_t)(((int16_t)data[1]<<8 | data[0]));
			destination[1] = (int16_t)(((int16_t)data[3]<<8 | data[2]));
			destination[2] = (int16_t)(((int16_t)data[5]<<8 | data[4]));
			UARTprintf("Magnet in x y z:%d, %d, %d \n", destination[0], destination[1], destination[2]);
			SysCtlDelay(10000);
		}
	}
}


void readAccel(int16_t *destination)
{
	uint8_t data[6];
	i2cReadData(MPU9250_ADDRESS, ACCEL_XOUT_H, 6, &data[0]);
	destination[0] = (int16_t)(((int16_t)data[0] << 8 | data[1]));
	destination[1] = (int16_t)(((int16_t)data[2] << 8 | data[3]));
	destination[2] = (int16_t)(((int16_t)data[4] << 8 | data[5]));
}

void readGyro(int16_t *destination)
{
	uint8_t data[6];
	i2cReadData(MPU9250_ADDRESS, GYRO_XOUT_H, 6, &data[0]);
	destination[0] = (int16_t)(((int16_t)data[0] << 8 | data[1]));
	destination[1] = (int16_t)(((int16_t)data[2] << 8 | data[3]));
	destination[2] = (int16_t)(((int16_t)data[4] << 8 | data[5]));
}


// Implementation of Sebastian Madgwick's "...efficient orientation filter for... inertial/magnetic sensor arrays"
// (see http://www.x-io.co.uk/category/open-source/ for examples and more details)
// which fuses acceleration, rotation rate, and magnetic moments to produce a quaternion-based estimate of absolute
// device orientation -- which can be converted to yaw, pitch, and roll. Useful for stabilizing quadcopters, etc.
// The performance of the orientation filter is at least as good as conventional Kalman-based filtering algorithms
// but is much less computationally intensive---it can be performed on a 3.3 V Pro Mini operating at 8 MHz!

void MadgwickQuaternionUpdate(float ax, float ay, float az, float gx, float gy, float gz, float mx, float my, float mz)
{
	float q1 = q[0], q2 = q[1], q3 = q[2], q4 = q[3];   // short name local variable for readability
	float norm;
	float hx, hy, _2bx, _2bz;
	float s1, s2, s3, s4;
	float qDot1, qDot2, qDot3, qDot4;

	// Auxiliary variables to avoid repeated arithmetic
	float _2q1mx;
	float _2q1my;
	float _2q1mz;
	float _2q2mx;
	float _4bx;
	float _4bz;
	float _2q1 = 2.0 * q1;
	float _2q2 = 2.0 * q2;
	float _2q3 = 2.0 * q3;
	float _2q4 = 2.0 * q4;
	float _2q1q3 = 2.0 * q1 * q3;
	float _2q3q4 = 2.0 * q3 * q4;
	float q1q1 = q1 * q1;
	float q1q2 = q1 * q2;
	float q1q3 = q1 * q3;
	float q1q4 = q1 * q4;
	float q2q2 = q2 * q2;
	float q2q3 = q2 * q3;
	float q2q4 = q2 * q4;
	float q3q3 = q3 * q3;
	float q3q4 = q3 * q4;
	float q4q4 = q4 * q4;

	// Normalise accelerometer measurement
	norm = sqrt(ax * ax + ay * ay + az * az);
	if (norm == 0.0) return; // handle NaN
	norm = 1.0/norm;
	ax *= norm;
	ay *= norm;
	az *= norm;

	// Normalise magnetometer measurement
	norm = sqrt(mx * mx + my * my + mz * mz);
	if (norm == 0.0) return; // handle NaN
	norm = 1.0/norm;
	mx *= norm;
	my *= norm;
	mz *= norm;

	// Reference direction of Earth's magnetic field
	_2q1mx = 2.0 * q1 * mx;
	_2q1my = 2.0 * q1 * my;
	_2q1mz = 2.0 * q1 * mz;
	_2q2mx = 2.0 * q2 * mx;
	hx = mx * q1q1 - _2q1my * q4 + _2q1mz * q3 + mx * q2q2 + _2q2 * my * q3 + _2q2 * mz * q4 - mx * q3q3 - mx * q4q4;
	hy = _2q1mx * q4 + my * q1q1 - _2q1mz * q2 + _2q2mx * q3 - my * q2q2 + my * q3q3 + _2q3 * mz * q4 - my * q4q4;
	_2bx = sqrt(hx * hx + hy * hy);
	_2bz = -_2q1mx * q3 + _2q1my * q2 + mz * q1q1 + _2q2mx * q4 - mz * q2q2 + _2q3 * my * q4 - mz * q3q3 + mz * q4q4;
	_4bx = 2.0 * _2bx;
	_4bz = 2.0 * _2bz;

	// Gradient decent algorithm corrective step
	s1 = -_2q3 * (2.0 * q2q4 - _2q1q3 - ax) + _2q2 * (2.0 * q1q2 + _2q3q4 - ay) - _2bz * q3 * (_2bx * (0.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q4 + _2bz * q2) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q3 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5 - q2q2 - q3q3) - mz);
	s2 = _2q4 * (2.0 * q2q4 - _2q1q3 - ax) + _2q1 * (2.0 * q1q2 + _2q3q4 - ay) - 4.0 * q2 * (1.0 - 2.0 * q2q2 - 2.0 * q3q3 - az) + _2bz * q4 * (_2bx * (0.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q3 + _2bz * q1) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q4 - _4bz * q2) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5 - q2q2 - q3q3) - mz);
	s3 = -_2q1 * (2.0 * q2q4 - _2q1q3 - ax) + _2q4 * (2.0 * q1q2 + _2q3q4 - ay) - 4.0 * q3 * (1.0 - 2.0 * q2q2 - 2.0 * q3q3 - az) + (-_4bx * q3 - _2bz * q1) * (_2bx * (0.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (_2bx * q2 + _2bz * q4) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + (_2bx * q1 - _4bz * q3) * (_2bx * (q1q3 + q2q4) + _2bz * (0.5 - q2q2 - q3q3) - mz);
	s4 = _2q2 * (2.0 * q2q4 - _2q1q3 - ax) + _2q3 * (2.0 * q1q2 + _2q3q4 - ay) + (-_4bx * q4 + _2bz * q2) * (_2bx * (0.5 - q3q3 - q4q4) + _2bz * (q2q4 - q1q3) - mx) + (-_2bx * q1 + _2bz * q3) * (_2bx * (q2q3 - q1q4) + _2bz * (q1q2 + q3q4) - my) + _2bx * q2 * (_2bx * (q1q3 + q2q4) + _2bz * (0.5 - q2q2 - q3q3) - mz);
	norm = sqrt(s1 * s1 + s2 * s2 + s3 * s3 + s4 * s4);    // normalise step magnitude
	norm = 1.0/norm;
	s1 *= norm;
	s2 *= norm;
	s3 *= norm;
	s4 *= norm;

	// Compute rate of change of quaternion
	qDot1 = 0.5 * (-q2 * gx - q3 * gy - q4 * gz) - beta * s1;
	qDot2 = 0.5 * (q1 * gx + q3 * gz - q4 * gy) - beta * s2;
	qDot3 = 0.5 * (q1 * gy - q2 * gz + q4 * gx) - beta * s3;
	qDot4 = 0.5 * (q1 * gz + q2 * gy - q3 * gx) - beta * s4;

	// Integrate to yield quaternion
	q1 += qDot1 * deltat;
	q2 += qDot2 * deltat;
	q3 += qDot3 * deltat;
	q4 += qDot4 * deltat;
	norm = sqrt(q1 * q1 + q2 * q2 + q3 * q3 + q4 * q4);    // normalise quaternion
	norm = 1.0/norm;
	q[0] = q1 * norm;
	q[1] = q2 * norm;
	q[2] = q3 * norm;
	q[3] = q4 * norm;

}
