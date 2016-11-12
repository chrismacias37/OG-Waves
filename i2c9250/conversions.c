/*
 * conversions.c
 *
 *  Created on: Nov 11, 2016
 *      Author: migpa
 */

#include "conversions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    fVal += 0.005;   // added after a comment from Matt McNabb, see below.

    dVal = fVal;
    dec = (int)(fVal * 100) % 100;

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

