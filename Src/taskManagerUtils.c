/*
 * taskManagerUtils.c
 *
 *  Created on: 25.10.2021
 *      Author: philipp
 */


#include "taskManagerUtils.h"
#ifndef STM32
#include <stdio.h>
#else
#include "uart.h"
#endif

uint8_t tryToUInt8(char * str,uint8_t* has_errors_ptr)
{
	if (str != 0)
	{
		return toUInt8(str);
	}
	else
	{
		printf("ERROR: tryToUInt8 got a NullPointer\n");
		*has_errors_ptr = 1;
		return 0;
	}
}

int16_t tryToInt16(char * str,uint8_t* has_errors_ptr)
{
	if (str != 0)
	{
		return toInt16(str);
	}
	else
	{
		printf("ERROR: tryToInt16 got a NullPointer\n");
		*has_errors_ptr = 1;
		return 0;
	}
}

uint8_t checkLampRange(uint8_t lampnr,uint8_t* has_errors_ptr)
{
	char nrbfr[4];
	if (lampnr < N_LAMPS)
	{
		return 1;
	}
	else
	{
		printf("ERROR: Lamp Nr ");
		UInt8ToChar(lampnr,nrbfr);
		printf(nrbfr);
		printf(" is out of range\r\n");
		*has_errors_ptr = 1;
		return 0;
	}
}
