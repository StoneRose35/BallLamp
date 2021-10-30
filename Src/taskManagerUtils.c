/*
 * taskManagerUtils.c
 *
 *  Created on: 25.10.2021
 *      Author: philipp
 */


#include "taskManagerUtils.h"
#include "intFunctions.h"
#include <string.h>
#include <stdlib.h>
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

uint8_t expandLampDescription(char * description,uint8_t * res)
{
	char * arrayElement;
	uint8_t * rangePtr;
	uint8_t rlength=0;
	uint8_t nlamps=0;
	uint8_t swapval;
	stripWhitespaces(description);
	arrayElement = strtok(description,",");
	//uint8_t lampsnrs[N_LAMPS];
	void* isInArray;
	while (arrayElement != NULL)
	{
		rlength = expandRange(arrayElement,&rangePtr);
		// insert into array
		for(uint8_t c=0;c<rlength;c++)
		{
			isInArray = bsearch((void*)(rangePtr+c),res,nlamps,sizeof(uint8_t),compareUint8);
			if (isInArray==NULL)
			{
				res[nlamps++]=rangePtr[c];
				for(uint8_t c2=nlamps-1;c2>0;c2--)
				{
					if(res[c2-1] > res[c2])
					{
						swapval = res[c2-1];
						res[c2-1] = res[c2];
						res[c2] = swapval;
					}
				}
			}
		}
		free(rangePtr);
		arrayElement = strtok(NULL,",");
	}
	//*res=lampsnrs;
	return nlamps;
}

void handleRgbStruct(RGB* clr,uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=clr->r;
	(lamps+nr)->rgb.b=clr->b;
	(lamps+nr)->rgb.g=clr->g;
}

void handleRgb(uint8_t r,uint8_t g, uint8_t b,uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=r;
	(lamps+nr)->rgb.b=b;
	(lamps+nr)->rgb.g=g;
}

