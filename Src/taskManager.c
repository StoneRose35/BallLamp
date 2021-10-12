/*
 * taskManager.c
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include "taskManager.h"
#include "neopixelDriver.h"
#include "system.h"
#ifdef STM32
#include "uart.h"
#else
#include <stdio.h>
#endif
#include "stringFunctions.h"
#include "intFunctions.h"
#include <string.h>
#include <stdlib.h>


const char * colorCommands[N_COMMANDS] = {
		"BACKGROUND",
		"FOREGROUND",
		"RED",
		"GREEN",
		"DARKBLUE",
		"LIGHTBLUE",
		"MAGENTA",
		"YELLOW",
		"ORANGE",
		"PURPLE",
		"YELLOWGREEN",
		"MEDIUMBLUE",
		"DARKYELLOW",
		"AQUA",
		"DARKPURPLE",
		"GRAY"};

RGB colors[] = {
		{.r=0,.g=0,.b=0}, // background
		{.r=255,.g=255,.b=255}, // foreground
		{.r=250,.g=60,.b=60}, // 2 red
		{.r=0,.g=220,.b=0}, // 3 green

		{.r=30,.g=60,.b=255}, // 4 dark bue
		{.r=0,.g=200,.b=200}, // 5 light blue
		{.r=240,.g=0,.b=130}, // 6 magenta
		{.r=230,.g=220,.b=50}, // 7 yellow

		{.r=240,.g=130,.b=40}, // 8 orange
		{.r=160,.g=0,.b=200}, // 9 purple
		{.r=160,.g=230,.b=50}, // 10 yellow/green
		{.r=0,.g=160,.b=255}, // 11 medium blue

		{.r=230,.g=175,.b=45}, // 12 dark yellow
		{.r=0,.g=210,.b=140}, // 13 aqua
		{.r=130,.g=0,.b=220}, // 14 dark purple
		{.r=170,.g=170,.b=170} // 15 gray
};

const char * rgbCommand = "RGB";


void handleCommand(char * cmd,RGBStream * lamps)
{
	uint8_t cmdFound = 0;
	char bracketContent[32];
	uint8_t * lampnrs;
	uint8_t nLamps;
	if (*cmd != 0)
	{
		for(uint8_t cnt=0;cnt<N_COMMANDS;cnt++)
		{
			if (startsWith(cmd,colorCommands[cnt])>0)
			{
				cmdFound=1;
				getBracketContent(cmd,bracketContent);
				nLamps = expandLampDescription(bracketContent,&lampnrs);
				for (uint8_t c=0;c<nLamps;c++)
				{
					handleRgbStruct(*(colors+cnt),*(lampnrs+c),lamps);
				}
			}
		}
		if (cmdFound == 0)
		{
			if (startsWith(cmd,rgbCommand)>0)
			{
				char * clr;
				const char comma[2]=",";
				uint8_t r,g,b;

				getBracketContent(cmd,bracketContent);

				clr = strtok(bracketContent,comma);
				r = toInt(clr);
				clr = strtok(0,comma);
				g = toInt(clr);
				clr = strtok(0,comma);
				b = toInt(clr);
				clr = strtok(0,"");
				uint8_t * lampnrs;

				nLamps = expandLampDescription(clr,&lampnrs);
				for (uint8_t c=0;c<nLamps;c++)
				{
					handleRgb(r,g,b,*(lampnrs+c),lamps);
				}

				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			if(startsWith(cmd,"help")>0)
			{
				handleHelp(0,0);
				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			printf("\r\nUnrecognizedCommandException");
			handleHelp(0,0);
		}
	}

}


void handleHelp(uint8_t nr,RGBStream * lamps)
{
	char nrbfr[4];
	printf("\r\nSupported Color commands are\r\n");
	for(uint8_t c=0;c<N_COMMANDS;c++)
	{
		printf(" * ");
		printf(colorCommands[c]);
		printf("(<LedNumbers>)\r\n");
	}
	printf(" * RGB(<red>,<green>,<blue>,<LedNumbers>)\r\n");
	printf("LedNumbers can be gives as any comma-separated list of lamp or ranges of lamps\r\n");
	printf("a range of lamps is defined as <lower>-<upper>, example: '3-6' would be\r\n");
	printf("equal to writing 3,4,5,6\r\n");
	UInt8ToChar(N_LAMPS,nrbfr);
	printf(nrbfr);
	printf("\r\n<r>, <g> and <b> range from 0 to 255\r\n");
	printf("example: RED(13) switches led 13 to red while leaving all others\r\n");
}

uint8_t expandLampDescription(char * description,uint8_t ** res)
{
	char * arrayElement;
	stripWhitespaces(description);
	arrayElement = strtok(description,",");
	uint8_t * rangePtr;
	uint8_t rlength=0;
	uint8_t nlamps=0;
	uint8_t swapval;
	uint8_t lampsnrs[N_LAMPS];
	void* isInArray;
	while (arrayElement != NULL)
	{
		rlength = expandRange(arrayElement,&rangePtr);
		// insert into array
		for(uint8_t c=0;c<rlength;c++)
		{
			isInArray = bsearch((void*)(rangePtr+c),lampsnrs,nlamps,sizeof(uint8_t),compareUint8);
			if (isInArray==NULL)
			{
				lampsnrs[nlamps++]=rangePtr[c];
				for(uint8_t c2=nlamps-1;c2>0;c2--)
				{
					if(lampsnrs[c2-1] > lampsnrs[c2])
					{
						swapval = lampsnrs[c2-1];
						lampsnrs[c2-1] = lampsnrs[c2];
						lampsnrs[c2] = swapval;
					}
				}
			}
		}
		free(rangePtr);
		arrayElement = strtok(NULL,",");
	}
	*res=lampsnrs;
	return nlamps;
}

void handleRgbStruct(RGB clr,uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=clr.r;
	(lamps+nr)->rgb.b=clr.b;
	(lamps+nr)->rgb.g=clr.g;
}

void handleRgb(uint8_t r,uint8_t g, uint8_t b,uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=r;
	(lamps+nr)->rgb.b=b;
	(lamps+nr)->rgb.g=g;
}





