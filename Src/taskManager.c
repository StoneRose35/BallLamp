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
		"GRAY",
		"help"};

RGB colors[] = {
		{.r=0,.g=0,.b=0},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255},
		{.r=255,.g=255,.b=255}
};


const void(*colorCommandFunctions[])(uint8_t,RGBStream*) ={
		handleBackground,
		handleForground,
		handleRed,
		handleGreen,
		handleDarkblue,
		handleLightblue,
		handleMagenta,
		handleYellow,
		handleOrange,
		handlePurple,
		handleYellowgreen,
		handleMediumblue,
		handleDarkyellow,
		handleAqua,
		handleDarkpurple,
		handleGray,
		handleHelp};
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
				clr = strtok(nrbfr,comma);
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

void handleBackground(uint8_t nr,RGBStream * lamps)
{
	handleRgb(0,0,0,nr,lamps);
}

void handleForground(uint8_t nr,RGBStream * lamps)
{
	handleRgb(255,255,255,nr,lamps);
}

void handleRed(uint8_t nr,RGBStream * lamps)
{
	handleRgb(250,60,60,nr,lamps);
}

void handleGreen(uint8_t nr,RGBStream * lamps)
{
	handleRgb(0,220,0,nr,lamps);
}

void handleDarkblue(uint8_t nr,RGBStream * lamps)
{
	handleRgb(30,60,255,nr,lamps);
}

void handleLightblue(uint8_t nr,RGBStream * lamps)
{
	handleRgb(0,200,200,nr,lamps);
}

void handleMagenta(uint8_t nr,RGBStream * lamps)
{
	handleRgb(240,0,130,nr,lamps);
}

void handleYellow(uint8_t nr,RGBStream * lamps)
{
	handleRgb(230,220,50,nr,lamps);
}

void handleOrange(uint8_t nr,RGBStream * lamps)
{
	handleRgb(230,130,40,nr,lamps);
}

void handlePurple(uint8_t nr,RGBStream * lamps)
{
	handleRgb(160,0,200,nr,lamps);
}

void handleYellowgreen(uint8_t nr,RGBStream * lamps)
{
	handleRgb(160,230,50,nr,lamps);
}

void handleMediumblue(uint8_t nr,RGBStream * lamps)
{
	handleRgb(0,160,255,nr,lamps);
}

void handleDarkyellow(uint8_t nr,RGBStream * lamps)
{
	handleRgb(0,160,255,nr,lamps);
}

void handleAqua(uint8_t nr,RGBStream * lamps)
{
	handleRgb(0,210,140,nr,lamps);
}

void handleDarkpurple(uint8_t nr,RGBStream * lamps)
{
	handleRgb(130,0,220,nr,lamps);
}

void handleGray(uint8_t nr,RGBStream * lamps)
{
	handleRgb(170,170,170,nr,lamps);
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





