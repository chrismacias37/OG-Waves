/*
 * main.c
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <inc/tm4c123gh6pm.h>

#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

#include <driverlib/sysctl.h>
#include <driverlib/rom.h>
#include <driverlib/i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include <driverlib/interrupt.h>
#include <driverlib/fpu.h>
#include <driverlib/pin_map.h>
#include <driverlib/hibernate.h>


#include "utils/conversions.h"
#include "utils/i2c.h"
#include "utils/bno055.h"

#include "utils/uart.h"
#include "utils/uartstdio.h"
#include "utils/bmp180.h"

int quaternion[4][2];
u8 data[1];
int i = 0;
int timestamp = 0;
float temperature;
int temp[2];
unsigned char error;
struct bno055_t bno;
struct bno055_euler_float_t euler;
struct bmp180_t bmp;
uint8_t sys, mag, accel, gyro;

u32 UP;
s32 pressure;
float height;
int alts[2];


uint32_t g_ui32SysClock;


int main(void) {
	
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

	uint32_t bnoPeriod;
	uint32_t bmpPeriod; //we will make this update every few seconds since it is so SLOW. (Takes 4.5 ms for an update).
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_A_PERIODIC);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	bnoPeriod = (SysCtlClockGet()/1000);
	bmpPeriod = (SysCtlClockGet())*5;
	TimerLoadSet(TIMER0_BASE, TIMER_A, bnoPeriod - 1);
	TimerLoadSet(TIMER1_BASE, TIMER_A, bmpPeriod - 1);



	delay(100);

	ConfigureUART();
	initI2C();
	FPUEnable();
	UARTprintf("Hello World!\n");

	/*
	 * Configure the GPIO port PA2 for power to the I2C bus
	 *
	 */

	/*
	 * Initialize the structures for the BNO055 and the BMP180
	 */

	bno.bus_read = i2cReadData;
	bno.bus_write = i2cWriteData;
	bno.delay_msec = delay;
	bno.dev_addr = BNO055_I2C_ADDR1;

	bmp.bus_read = i2cReadData;
	bmp.bus_write = i2cWriteData;
	bmp.delay_msec = delay;
	bmp.number_of_samples = 8;
	bmp.dev_addr = BMP180_I2C_ADDR;


	//init the bno055
	bno055_init(&bno);
	bno055_set_operation_mode(BNO055_OPERATION_MODE_NDOF);
	i2cReadData(0x28, 0x00, data, 1);
	UARTprintf("Chip ID: %d\n", data[0]);

	bno055_convert_float_temp_fahrenheit(&temperature);
	convertFloat(temperature, temp);
	UARTprintf("Temp: %d.%d\n", temp[0], temp[1]);

	bmp180_init(&bmp);
	bmp180_get_calib_param();




	UARTprintf("I am: %d\n", data[0]);



	IntEnable(INT_TIMER0A);
	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE,TIMER_A);
	TimerEnable(TIMER1_BASE, TIMER_A);


	if(data[0] == 160)
	{
		while(1)
		{

		}
	}



	}


void Timer0IntHandler(void)
{

	error = bno055_convert_float_euler_hpr_deg(&euler);
	bno055_get_mag_calib_stat(&mag);
	bno055_get_accel_calib_stat(&accel);
	bno055_get_gyro_calib_stat(&gyro);
	bno055_get_sys_calib_stat(&sys);
	bno055_convert_float_temp_fahrenheit(&temperature);
	UP = bmp180_get_uncomp_pressure();
	pressure = bmp180_get_pressure(UP);
	height = altitude(pressure);
	convertFloat(height, alts);

	if(error == 0x00)
	{
		convertFloat(euler.h, quaternion[0]);
		convertFloat(euler.p, quaternion[1]);
		convertFloat(euler.r, quaternion[2]);
		convertFloat(temperature, temp);
		UARTprintf("Orientation: %d.%d %d.%d %d.%d\n", quaternion[0][0], quaternion[0][1],quaternion[1][0], quaternion[1][1],quaternion[2][0], quaternion[2][1]);
		UARTprintf("Calibration: %d %d %d %d\n", sys, gyro, accel, mag); //show calibration status
		UARTprintf("Temp: %d.%d\n", temp[0], temp[1]);
		UARTprintf("Alt: %d.%d\n", alts[0], alts[1]);
		UARTprintf("Pressure: %d\n" ,pressure);
	}
	else
	{
		UARTprintf("Something is wrong...");
	}
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

}

void Timer1IntHandler(void)
{

	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);


}

