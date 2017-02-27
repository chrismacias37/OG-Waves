/*
 * conversions.h
 *
 *  Created on: Nov 11, 2016
 *      Author: migpa
 */

#ifndef UTILS_CONVERSIONS_H_
#define UTILS_CONVERSIONS_H_
#include <stdint.h>


void float2String(float fVal, char *out);
void ftoa(float n, char *res, int afterpoint);
void reverse(char *str, int len);
int intToStr(int x, char str[], int d);
void convertFloat(float x, int* result);
float altitude(int32_t pressure);

#endif /* UTILS_CONVERSIONS_H_ */
