/*
 * main.c
 */
#include"hardware/gpio.h"
#include"hardware/i2c.h"

int main(void) {

	initalize_pins();
	initalize_i2c();

}
