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
#include "interpolators.h"
#include "taskManagerUtils.h"
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
const char * stopCommand = "STOP";
const char * startCommand = "START";
const char * listCommand = "LIST";

extern TasksType interpolators;


void handleCommand(char * cmd,RGBStream * lamps)
{
	uint8_t cmdFound = 0;
	char bracketContent[32];
	uint8_t * lampnrs;
	uint8_t nLamps;
	char nrbfr[4];
	uint8_t retval;
	uint8_t has_errors = 0;
	if (*cmd != 0)
	{
		printf("\r\n");
		for(uint8_t cnt=0;cnt<N_COMMANDS;cnt++)
		{
			if (startsWith(cmd,colorCommands[cnt])>0)
			{
				has_errors = 0;
				cmdFound=1;
				getBracketContent(cmd,bracketContent);
				if (bracketContent != 0)
				{
					nLamps = expandLampDescription(bracketContent,&lampnrs);
					for (uint8_t c=0;c<nLamps;c++)
					{
						if (checkLampRange(*(lampnrs+c),&has_errors) == 1)
						{
							handleRgbStruct(*(colors+cnt),*(lampnrs+c),lamps);
						}
					}
				}
				else
				{
					printf("ERROR: no content within brackets \"()\" found\r\n");
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
				has_errors = 0;
				getBracketContent(cmd,bracketContent);
				if (bracketContent != 0)
				{
					clr = strtok(bracketContent,comma);
					r = tryToUInt8(clr,&has_errors);
					clr = strtok(0,comma);
					g = tryToUInt8(clr,&has_errors);
					clr = strtok(0,comma);
					b = tryToUInt8(clr,&has_errors);
					if (has_errors == 1)
					{
						printf("ERROR: invalid number of arguments\r\n");
					}
					clr = strtok(0,"");
					uint8_t * lampnrs;
					if (clr != 0 && has_errors == 0)
					{
						nLamps = expandLampDescription(clr,&lampnrs);
						for (uint8_t c=0;c<nLamps;c++)
						{
							if (checkLampRange(*(lampnrs+c),&has_errors)==1)
							{
								handleRgb(r,g,b,*(lampnrs+c),lamps);
							}
						}
					}
					else
					{
						printf("ERROR: invalid number of arguments\r\n");
					}
				}
				else
				{
					printf("ERROR: no content within brackets \"()\" found\r\n");
				}

				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			if (startsWith(cmd,startCommand)>0)
			{
				has_errors = 0;
				startInterpolators(&interpolators);
				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			if (startsWith(cmd,stopCommand)>0)
			{
				has_errors = 0;
				stopInterpolators(&interpolators);
				cmdFound = 1;
			}

		}
		if (cmdFound == 0)
		{
			if(startsWith(cmd,"HELP")>0)
			{
				has_errors = 0;
				handleHelp(0,0);
				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			if(startsWith(cmd,"INTERP") > 0)
			{
				has_errors = 0;
				char * var;
                uint8_t lampnr;
                uint8_t nsteps;
                uint8_t repeating;
				getBracketContent(cmd,bracketContent);
				if (bracketContent != 0)
				{
					var = strtok(bracketContent,",");
					lampnr = tryToUInt8(var,&has_errors);
					if (has_errors == 0)
					{
						checkLampRange(lampnr,&has_errors);
					}
					var = strtok(0,",");
					nsteps = tryToUInt8(var,&has_errors);
					var = strtok(0,",");
					repeating = tryToUInt8(var,&has_errors);
					if (repeating > 1)
					{
						printf("ERROR: repeating has to be either 0 or 1\r\n");
						has_errors = 1;
					}
					if (repeating == 1 && nsteps == 1)
					{
						printf("ERROR: a repeating color sequence with only one color\r\n");
						printf("       is meaningless, use RGB for setting a constant color\r\n");
						printf("       or set <repeating> to 0\r\n");
						has_errors = 1;
					}
					if (has_errors == 0)
					{
						retval = setLampInterpolator(&interpolators,lampnr,nsteps,repeating);
						if (retval != 0)
						{
							printf("ERROR: returned error code ");
							UInt8ToChar(retval, nrbfr);
							printf(nrbfr);
							printf(" from setLampInterpolator\r\n");
						}
					}
				}
				else
				{
					printf("ERROR: no content within brackets \"()\" found\r\n");
				}
				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			if(startsWith(cmd,"ISTEP") > 0)
			{
				char * var;
				uint8_t r,g,b;
				int16_t frames;
				uint8_t interpolation;
				uint8_t lampnr;
				uint8_t step;
				has_errors = 0;
				getBracketContent(cmd,bracketContent);
				if (bracketContent != 0)
				{
					var = strtok(bracketContent,",");
					r = tryToUInt8(var,&has_errors);
					var = strtok(0,",");
					g = tryToUInt8(var,&has_errors);
					var = strtok(0,",");
					b = tryToUInt8(var,&has_errors);
					var = strtok(0,",");
					frames = tryToInt16(var,&has_errors);
					var = strtok(0,",");
					interpolation = tryToUInt8(var,&has_errors);

					if (interpolation > 1)
					{
						printf("ERROR: interpolation must be either 0 for constant or 1 for linear\r\n");
						has_errors = 1;
					}

					var = strtok(0,",");
					lampnr = tryToUInt8(var,&has_errors);
					checkLampRange(lampnr,&has_errors);
					var = strtok(0,",");
					step = tryToUInt8(var,&has_errors);

					if (has_errors == 0)
					{
						retval = setColorFramesInterpolation(&interpolators,r,g,b,frames,interpolation,lampnr,step);
						if (retval != 0)
						{
							printf("ERROR: returned error code ");
							UInt8ToChar(retval, nrbfr);
							printf(nrbfr);
							printf(" from setColorFramesInterpolation\r\n");
						}
					}
				}
				else
				{
					printf("ERROR: no content within brackets \"()\" found\r\n");
				}
				cmdFound=1;
			}
		}
		if (cmdFound == 0)
		{
			if(startsWith(cmd,"DESCI")>0)
			{
				has_errors = 0;
				describeInterpolators();
				cmdFound = 1;
			}
		}
		if (cmdFound == 0)
		{
			if(startsWith(cmd,"DESTROY"))
			{
				has_errors = 0;
				uint8_t lampnr;
				uint8_t lampidx;
				getBracketContent(cmd,bracketContent);
				if (bracketContent != 0)
				{
					lampnr = tryToUInt8(bracketContent,&has_errors);
					if (checkLampRange(lampnr,&has_errors) != 0)
					{
						lampidx = getLampIndex(&interpolators,lampnr);
						if (lampidx != 0xFF)
						{
							destroyTask(interpolators.taskArray+lampidx);
						}
						else
						{
							printf("ERROR: no interpolator found to destroy\r\n");
						}
					}
				}
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

void describeInterpolators()
{
	char nrbfr[8];
	for(uint8_t c=0;c<interpolators.taskArrayLength;c++)
	{
		if (interpolators.taskArray[c].steps != 0)
		{
			printf("Color-Sequence ");
			UInt8ToChar(c,nrbfr);
			printf(nrbfr);
			printf("\r\n");
			printf("   Steps: ");
			UInt8ToChar(interpolators.taskArray[c].Nsteps,nrbfr);
			printf(nrbfr);
			printf(", Lamp: ");
			UInt8ToChar(interpolators.taskArray[c].lamp_nr,nrbfr);
			printf(nrbfr);
			if((interpolators.taskArray[c].state & 0x4) == 0x4)
			{
				printf("\r\n   Mode: repeating");
			}
			else
			{
				printf("\r\n   Mode: one-shot");
			}
			printf(", State: ");
			if((interpolators.taskArray[c].state & 0x2) == 0x2)
			{
				printf("running");
			}
			else
			{
				printf("stopped/starting");
			}
			printf(", Progression: ");
			toPercentChar(getProgression(interpolators.taskArray+c),nrbfr);
			printf(nrbfr);
			printf("%\r\n\r\n");
		}
	}
}


void handleHelp(uint8_t nr,RGBStream * lamps)
{
	printf("Supported Color commands are\r\n");
	for(uint8_t c=0;c<N_COMMANDS;c++)
	{
		printf(" * ");
		printf(colorCommands[c]);
		printf("(<LedNumbers>)\r\n");
	}
	printf(" * RGB(<red>,<green>,<blue>,<LedNumbers>)\r\n");
	printf("   LedNumbers can be gives as any comma-separated list of lamp or ranges of lamps\r\n");
	printf("   a range of lamps is defined as <lower>-<upper>, example: '3-6' would be\r\n");
	printf("   equal to writing 3,4,5,6\r\n");
	printf("   <red>, <green> and <blue> range from 0 to 255\r\n");
	printf("   example: RED(13) switches led 13 to red while leaving all others\r\n");
	printf(" * START: starts all interpolators\r\n");
	printf(" * STOP: stops all interpolators\r\n");
	printf(" * INTERP(<lamp_nr>,<nSteps>,<repeating>): defines an interpolator/color sequence for Lamp <lampnr>\r\n");
	printf("   with <nSteps> steps, loops if <repeating> is 1, executes as one-shot if 0 \r\n");
	printf(" * ISTEP(<r>,<g>,<b>,<frames>,<interpolation>,<lampnr>,<step>)\r\n");
	printf("   with <r>, <g>, <b> being the colors, <frames> the number of frames the step should last\r\n");
	printf("   <interpolation> is 0 for constant, 1 for linear interpolation to the next value\r\n");
	printf("   <lampnr> is the lamp nr,<step> is the step to define\r\n");
	printf("   make sure to call INTERP prior to defining individual steps\r\n");
	printf(" * DESCI: returns a description of all initialized interpolators\r\n");
	printf(" * DESTROY(<lampnr>) removes an interpolator for a certain lamp nr\r\n");
	printf("   does nothing if the interpolator is not initialized\r\n");

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





