/*
 * sdCardAdapter.c
 *
 *  Created on: Nov 4, 2016
 *      Author: ChristianMacias
 */
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include"sdCardAdapter.h"
#include"SD_Card/sd"

int mmc_init(int base_address)
{
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 200000, 64);//Use SSI in Motorola mode 0 (AKA SPI), set clock rate to 200kHz, and set 64bit frame.
	SSIEnable(SSI0_BASE);
}
