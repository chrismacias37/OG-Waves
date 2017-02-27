/*
 * conversions.c
 *
 *  Created on: Nov 11, 2016
 *      Author: migpa
 */

#include "conversions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#define R 8.31432
#define g0 9.80665
#define M 0.0289644
#define lb -0.0065
#define tb 288.15


//improve this function later.
//this only has an effect on the visualization.
void float2String(float fVal, char *out)
{
	int negative = 0;
	if(fVal < 0)
	{
		//need to handle negative numbers
		negative = 1;
		fVal *= -1;
	}
    char result[100];
    int dVal, dec, i;

    //fVal += 0.005;   // added after a comment from Matt McNabb, see below.

    dVal = fVal;
    dec = (int)(fVal * 1000) % 1000;

    memset(result, 0, 100);
    result[0] = (dec % 10) + '0';
    result[1] = (dec / 10) + '0';
    result[2] = '.';

    i = 3;
    while (dVal > 0)
    {
        result[i] = (dVal % 10) + '0';
        dVal /= 10;
        i++;
    }

    //before sending back result, we must reverse the order of the string;
    int length = strlen(result);
    int q = 0;
    if(negative == 0)
    {
    for(q = 0; q<length; q++)
    {
    	//reverse the string
    	out[q] = result[(length-1)-q];
    }
    }
    else if(negative == 1)
    {
    	out[0] = '-';
    	for(q = 1; q<length; q++)
    	    {
    	    	//reverse the string
    	    	out[q] = result[(length)-q];
    	    }
    }

}
// Converts a floating point number to string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.';  // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter is needed
        // to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

// reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

 // Converts a given integer x to string str[].  d is the number
 // of digits required in output. If d is more than the number
 // of digits in x, then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

void convertFloat(float x, int* result)
{
	int part1;
	float temp;

	part1 = (int)x;
	temp = (x - (float)part1) * 1000; //stores 4 significant digits
	result[0] = part1;
	result[1] = abs((int)temp);

	//now you just have to format it accordingly.


}

float altitude(int32_t pressure)
{
	pressure = pressure;
	float ratio = (float)pressure /101325.0;
	float temp = 0.0;
	temp = pow(ratio, 0.190294957);
	float alt = 44330.0*(1.0-temp);
	return alt - 553;
}
float alt2(int32_t pressure)
{
	/*
	float pressureRatio = (float)pressure /101325.0;
	float tempRatio = tb/lb;
	float powerRatio = ((-R*lb)/(g0*M));
	float temp = pow(pressureRatio, powerRatio);
	float alt = tempRatio*(temp-1);
	return alt;
*/
}

