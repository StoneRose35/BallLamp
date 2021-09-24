/*
 * taskManager.c
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include "taskManager.h"
#include "neopixelDriver.h"
#ifdef STM32
#include "uart.h"
#else
#include <stdio.h>
#endif
#include "stringFunctions.h"
#include <string.h>
#include <stdlib.h>



const char * colorCommands[N_COMMANDS] = {"OFF","WHITE","RED","BLUE","GREEN","--help"};
const void(*colorCommandFunctions[])(uint8_t,RGBStream*) ={handleOff, handleWhite, handleRed, handleBlue, handleGreen, handleHelp};
const char * rgbCommand = "RGB";


void handleCommand(char * cmd,RGBStream * lamps)
{
	uint8_t cmdFound = 0;
	char nrbfr[16];
	if (*cmd != 0)
	{
		for(uint8_t cnt=0;cnt<N_COMMANDS;cnt++)
		{
			if (startsWith(cmd,colorCommands[cnt])>0)
			{
				cmdFound=1;
				if (colorCommandFunctions[cnt] != 0)
				{
					getBracketContent(cmd,nrbfr);
					if (nrbfr != 0)
					{
						(*colorCommandFunctions[cnt])(toInt(nrbfr),lamps);
					}
				}
				else
				{
					printf("\r\nNotImplementedException");
				}
			}
		}
		if (cmdFound == 0)
		{
			if (startsWith(cmd,rgbCommand)>0)
			{
				char * clr;
				const char comma[2]=",";
				uint8_t r,g,b,lampnr;

				getBracketContent(cmd,nrbfr);

				//split(nrbfr,',',subColors);
				clr = strtok(cmd,comma);
				r = toInt(clr);
				clr = strtok(0,comma);
				g = toInt(clr);
				clr = strtok(0,comma);
				b = toInt(clr);
				clr = strtok(0,comma);
				lampnr = toInt(clr);
				handleRgb(r,g,b,lampnr,lamps);
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

void handleOff(uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=0;
	(lamps+nr)->rgb.b=0;
	(lamps+nr)->rgb.g=0;
}

void handleWhite(uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=0xFF;
	(lamps+nr)->rgb.b=0xFF;
	(lamps+nr)->rgb.g=0xFF;
}

void handleRed(uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=0xFF;
	(lamps+nr)->rgb.b=0x00;
	(lamps+nr)->rgb.g=0x00;
}

void handleGreen(uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=0x00;
	(lamps+nr)->rgb.b=0x00;
	(lamps+nr)->rgb.g=0xFF;
}

void handleBlue(uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=0x00;
	(lamps+nr)->rgb.b=0xFF;
	(lamps+nr)->rgb.g=0x00;
}
void handleHelp(uint8_t nr,RGBStream * lamps)
{
	char nrbfr[4];
	printf("\r\nSupported Color commands are\r\n");
	for(uint8_t c=0;c<N_COMMANDS;c++)
	{
		printf(" * ");
		printf(colorCommands[c]);
		printf("(<LedNumber>)\r\n");
	}
	printf(" * RGB(<red>,<green>,<blue>,<LedNumber>)\r\n");
	printf("LedNumber is from 0 to ");
	UInt8ToChar(N_LAMPS,nrbfr);
	printf(nrbfr);
	printf("\r\n<r>, <g> and <b> range from 0 to 255\r\n");
	printf("example: RED(13) switches led 13 to red while leaving all others\r\n");
}

void handleRgb(uint8_t r,uint8_t g, uint8_t b,uint8_t nr,RGBStream * lamps)
{
	(lamps+nr)->rgb.r=r;
	(lamps+nr)->rgb.b=b;
	(lamps+nr)->rgb.g=g;
}





