/*
 * taskManager.c
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include "taskManager.h"
#include "neopixelDriver.h"
#include "uart.h"
#include "stringFunctions.h"
#include <string.h>
#include <stdlib.h>



extern RGBStream * frame;
const char * colorCommands[N_COMMANDS] = {"OFF","WHITE","RED","BLUE","GREEN","--help"};
const void(*colorCommandFunctions[])(uint8_t) ={handleOff, handleWhite, handleRed, handleBlue, handleGreen, handleHelp};
const char * rgbCommand = "RGB";


void handleCommand(const char * cmd)
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
						(*colorCommandFunctions[cnt])(toInt(nrbfr));
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
				char subColors[4*4];

				getBracketContent(cmd,nrbfr);
				split(nrbfr,',',subColors);
				uint8_t r,g,b;
				r = toInt(subColors);
				g = toInt(subColors+4);
				b = toInt(subColors+8);
				handleRgb(r,g,b,toInt(subColors+12));
				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			printf("\r\nUnrecognizedCommandException");
			handleHelp(0);
		}
	}

}

void handleOff(uint8_t nr)
{
	(frame+nr)->rgb.r=0;
	(frame+nr)->rgb.b=0;
	(frame+nr)->rgb.g=0;
}

void handleWhite(uint8_t nr)
{
	(frame+nr)->rgb.r=0xFF;
	(frame+nr)->rgb.b=0xFF;
	(frame+nr)->rgb.g=0xFF;
}

void handleRed(uint8_t nr)
{
	(frame+nr)->rgb.r=0xFF;
	(frame+nr)->rgb.b=0x00;
	(frame+nr)->rgb.g=0x00;
}

void handleGreen(uint8_t nr)
{
	(frame+nr)->rgb.r=0x00;
	(frame+nr)->rgb.b=0x00;
	(frame+nr)->rgb.g=0xFF;
}

void handleBlue(uint8_t nr)
{
	(frame+nr)->rgb.r=0x00;
	(frame+nr)->rgb.b=0xFF;
	(frame+nr)->rgb.g=0x00;
}
void handleHelp(uint8_t nr)
{
	char nrbfr[4];
	printf("\r\nSupported Color commands are\r\n");
	for(uint8_t c;c<N_COMMANDS;c++)
	{
		printf(" * ");
		printf(colorCommands[c]);
		printf("(<LedNumber>)\r\n");
	}
	printf(" * RGB(<red>,<green>,<blue>,<LedNumber>)\r\n");
	printf("LedNumber is from 0 to ");
	toChar(N_LAMPS,nrbfr);
	printf(nrbfr);
	printf("\r\n<r>, <g> and <b> range from 0 to 255\r\n");
	printf("example: RED(13) switches led 13 to red while leaving all others\r\n");
}

void handleRgb(uint8_t r,uint8_t g, uint8_t b,uint8_t nr)
{
	(frame+nr)->rgb.r=r;
	(frame+nr)->rgb.b=b;
	(frame+nr)->rgb.g=g;
}





