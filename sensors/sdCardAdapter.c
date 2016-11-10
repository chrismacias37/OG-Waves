/*
 * sdCardAdapter.c
 *
 *  Created on: Nov 4, 2016
 *      Author: ChristianMacias
 */
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include"sdCardAdapter.h"

int mmc_init(int base_address)
{
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 200, 11);
	SSIEnable(SSI0_BASE);
}
