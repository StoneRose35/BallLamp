/*
 * taskManager.c
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */
#include "systemChoice.h"
#include "taskManager.h"
#include "system.h"
#include "memoryAccess.h"
#include "flash.h"
#ifdef HARDWARE
#include "uart.h"
#else
#include <stdio.h>
#endif
#include "stringFunctions.h"
#include "interpolators.h"
#include "taskManagerUtils.h"
#include <bufferedInputHandler.h>
#include "bluetoothATConfig.h"

#include <string.h>
#include <stdlib.h>


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

const char * colorCommands[N_COLOR_COMMANDS] = {
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

void colorCommand(char * cmd,void * context)
{
	char bracketContent[128];
	uint8_t nLamps;
	uint8_t lampnrs[N_LAMPS];
	uint8_t has_errors = 0;

	uint8_t cnt=0,idx=0xFF;
	RGBStream* lamps =(RGBStream*)context;

	for (cnt=0;cnt<N_COLOR_COMMANDS;cnt++)
	{
		if (startsWith(cmd,colorCommands[cnt]) > 0)
		{
			idx = cnt;
		}
	}
	getBracketContent(cmd,bracketContent);
	if (bracketContent != 0 && idx != 0xFF)
	{
		nLamps = expandLampDescription(bracketContent,lampnrs);
		for (uint8_t c=0;c<nLamps;c++)
		{
			if (checkLampRange(*(lampnrs+c),&has_errors) == 1)
			{
				handleRgbStruct((colors+idx),*(lampnrs+c),lamps);
			}
		}
	}
	else
	{
		printf("ERROR: no content within brackets \"()\" found\r\n");
	}
}

void rgbCommand(char* cmd,void* context)
{
	char * clr;
	const char comma[2]=",";
	uint8_t r,g,b;
	uint8_t has_errors = 0;
	char bracketContent[128];
	uint8_t lampnrs[N_LAMPS];
	uint8_t nLamps;
	RGBStream* lamps =(RGBStream*)context;
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

		if (clr != 0 && has_errors == 0)
		{
			nLamps = expandLampDescription(clr,lampnrs);
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
}



void startCommand(char * cmd,void* context)
{
	Tasks interpolators=(Tasks)context;
	startInterpolators(interpolators);
}

void stopCommand(char * cmd,void* context)
{
	Tasks interpolators=(Tasks)context;
	stopInterpolators(interpolators);
}

void helpCommand(char * cmd,void* context)
{
	printf("Supported Color commands are\r\n");
	for(uint8_t c=0;c<N_COLOR_COMMANDS;c++)
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
	printf(" * INTERP(<lamp_nr>,<nSteps>,<repeating>): defines an interpolator/color sequence for Lamp <lampnr>, 255 means every lamp\r\n");
	printf("   with <nSteps> steps, loops if <repeating> is 1, executes as one-shot if 0 \r\n");
	printf(" * ISTEP(<r>,<g>,<b>,<frames>,<interpolation>,<lampnr>,<step>)\r\n");
	printf("   with <r>, <g>, <b> being the colors, <frames> the number of frames the step should last\r\n");
	printf("   <interpolation> is 0 for constant, 1 for linear interpolation to the next value\r\n");
	printf("   <lampnr> is the lamp nr, 255 means every lamp,<step> is the step to define\r\n");
	printf("   make sure to call INTERP prior to defining individual steps\r\n");
	printf(" * DESCI: returns a description of all initialized interpolators\r\n");
	printf(" * DESTROY(<lampnr>) removes an interpolator for a certain lamp nr or all lamps if 255 is give as a lamp nr\r\n");
	printf("   does nothing if the interpolator is not initialized\r\n");
	printf(" * SAVE: persistently saves the interpolators\r\n");
	printf(" * LOAD: loads the interpolators\r\n");
	printf(" * API: switches physical interface to api mode, does not echo character and does not allow \r\n");
	printf("   command line editing and history\r\n");
	printf(" * CONSOLE: switches the physical interface to console mode, characters received are echoed\r\n");
	printf("   also command line history and editing is possible\r\n");
	printf(" * SETUPBT: changes the bluetooth interface name and pin, sets bluetooth speed to 57600\r\n");

}


void interpCommand(char * cmd,void* context)
{
	char * var;
	uint8_t lampnr;
	uint8_t nsteps;
	uint8_t repeating, has_errors = 0, retval;
	char bracketContent[128];
	char nrbfr[4];
	Tasks interpolators=(Tasks)context;
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
			if (lampnr < 0xFF)
			{
				retval = setLampInterpolator(interpolators,lampnr,nsteps,repeating);
			}
			else
			{
				retval=0;
				for (uint8_t cc=0;cc<N_LAMPS;cc++)
				{
					retval += setLampInterpolator(interpolators,cc,nsteps,repeating);
				}
			}
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
}


void istepCommand(char * cmd,void* context)
{
	char * var;
	uint8_t r,g,b;
	uint32_t frames;
	uint8_t interpolation;
	uint8_t lampnr;
	uint8_t step, retval;
	uint8_t has_errors = 0;
	char bracketContent[128];
	char nrbfr[4];
	getBracketContent(cmd,bracketContent);
	Tasks interpolators=(Tasks)context;
	if (bracketContent != 0)
	{
		var = strtok(bracketContent,",");
		r = tryToUInt8(var,&has_errors);
		var = strtok(0,",");
		g = tryToUInt8(var,&has_errors);
		var = strtok(0,",");
		b = tryToUInt8(var,&has_errors);
		var = strtok(0,",");
		frames = tryToUInt32(var,&has_errors);
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
			if (lampnr < 0xFF)
			{
				retval = setColorFramesInterpolation(interpolators,r,g,b,frames,interpolation,lampnr,step);
			}
			else
			{
				retval=0;
				for(uint8_t cc=0;cc<N_LAMPS;cc++)
				{
					retval += setColorFramesInterpolation(interpolators,r,g,b,frames,interpolation,cc,step);
				}
			}
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

}

void desciCommand(char * cmd,void* context)
{
	char nrbfr[8];
	Tasks interpolators=(Tasks)context;
	printf("\r\n");
	for(uint8_t c=0;c<interpolators->taskArrayLength;c++)
	{
		if (interpolators->taskArray[c].steps != 0)
		{
			printf("Color-Sequence ");
			UInt8ToChar(c,nrbfr);
			printf(nrbfr);
			printf("\r\n");
			printf("   Steps: ");
			UInt8ToChar(interpolators->taskArray[c].Nsteps,nrbfr);
			printf(nrbfr);
			printf(", Lamp: ");
			UInt8ToChar(interpolators->taskArray[c].lamp_nr,nrbfr);
			printf(nrbfr);
			if((interpolators->taskArray[c].state & 0x4) == 0x4)
			{
				printf("\r\n   Mode: repeating");
			}
			else
			{
				printf("\r\n   Mode: one-shot");
			}
			printf(", State: ");
			if((interpolators->taskArray[c].state & 0x2) == 0x2)
			{
				printf("running");
			}
			else
			{
				printf("stopped/starting");
			}
			printf(", Progression: ");
			toPercentChar(getProgression(interpolators->taskArray+c),nrbfr);
			printf(nrbfr);
			printf("%\r\n");
		}
	}
}


void destroyCommand(char * cmd,void* context)
{
	uint8_t has_errors = 0;
	uint8_t lampnr;
	uint8_t lampidx;
	char bracketContent[128];
	getBracketContent(cmd,bracketContent);
	Tasks interpolators=(Tasks)context;
;	if (bracketContent != 0)
	{
		lampnr = tryToUInt8(bracketContent,&has_errors);
		if (checkLampRange(lampnr,&has_errors) != 0)
		{
			if(lampnr < 0xFF)
			{
				lampidx = getLampIndex(interpolators,lampnr);
				if (lampidx != 0xFF)
				{
					destroyTask(interpolators->taskArray+lampidx);
				}
				else
				{
					printf("ERROR: no interpolator found to destroy\r\n");
				}
			}
			else
			{
				for(uint8_t cc=0;cc<N_LAMPS;cc++)
				{
					lampidx = getLampIndex(interpolators,cc);
					if (lampidx != 0xFF)
					{
						destroyTask(interpolators->taskArray+lampidx);
					}
					else
					{
						printf("ERROR: no interpolator found to destroy\r\n");
					}
				}
			}
		}
	}
}

void saveCommand(char * cmd,void* context)
{
	uint16_t retcode = 0;
	uint32_t streamsize;
	char nrbfr[8];

	Tasks interpolators=(Tasks)context;
	uint8_t * streamout;
	streamsize = toStream(interpolators,&streamout);
	retcode = saveData((uint16_t*)streamout,streamsize,FLASH_HEADER_SIZE);
	free(streamout);
	printf("\r\nsaved ");
	UInt32ToChar(streamsize,nrbfr);
	printf(nrbfr);
	printf(" bytes to flash, returned code: ");
	UInt16ToChar(retcode,nrbfr);
	printf(nrbfr);
	printf("\r\n");
}

void loadCommand(char * cmd,void* context)
{
	Tasks interpolators=(Tasks)context;
	for (uint8_t c=0;c<interpolators->taskArrayLength;c++)
	{
		destroyTask(interpolators->taskArray+c);
	}
	fromStream((uint16_t*)((ptr)getFilesystemStart()+FLASH_HEADER_SIZE),interpolators);
}

void apiCommand(char * cmd,void* context)
{
	BufferedInput binput = (BufferedInput)context;
	binput->interfaceType = BINPUT_TYPE_API;
	printf("APIOK\n");
	binput->console->outBfr[0]=0; // disable command prompt already generated by the console handler
}

void consoleCommand(char * cmd,void* context)
{
	BufferedInput binput = (BufferedInput)context;
	binput->interfaceType = BINPUT_TYPE_CONSOLE;
	printf("Switching back to Console Mode\r\n");
}

void setupBluetoothCommand(char * cmd,void* context)
{
	if(ATCheckEnabled() == 0)
	{
		ATSetName("BallLamp");
		ATSetPin("3006");
		ATSetBaud("7");
	}
	else
	{
		printf("Can't configure Bluetooth: already paired\r\n");
	}
}

UserCommandType userCommands[] = {
	{"BACKGROUND",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"FOREGROUND",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"RED",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"GREEN",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"DARKBLUE",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"LIGHTBLUE",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"MAGENTA",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"YELLOW",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"ORANGE",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"PURPLE",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"YELLOWGREEN",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"MEDIUMBLUE",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"DARKYELLOW",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"AQUA",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"DARKPURPLE",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"GRAY",&colorCommand,CONTEXT_TYPE_RGBSTREAM},
	{"RGB",&rgbCommand,CONTEXT_TYPE_RGBSTREAM},
	{"START",&startCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"STOP",&stopCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"SAVE",&saveCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"LOAD",&loadCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"INTERP",&interpCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"ISTEP",&istepCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"DESCI",&desciCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"DESTROY",&destroyCommand,CONTEXT_TYPE_INTERPOLATORS},
	{"API",&apiCommand,CONTEXT_TYPE_BUFFEREDINPUT},
	{"CONSOLE",&consoleCommand,CONTEXT_TYPE_BUFFEREDINPUT},
	{"SETUPBT",&setupBluetoothCommand,CONTEXT_TYPE_NONE},
	{"HELP",&helpCommand,CONTEXT_TYPE_NONE},
	{"0",0}
};



void callUserFunction(void(*userFct)(char*,void*),char *cmd,uint8_t contextType,void* callerContext)
{
	extern TasksType interpolators;
	extern RGBStream * lamps;

	switch(contextType)
	{
	case CONTEXT_TYPE_RGBSTREAM:
		(*userFct)(cmd,(void*)lamps);
		break;
	case CONTEXT_TYPE_INTERPOLATORS:
		(*userFct)(cmd,(void*)&interpolators);
		break;
	case CONTEXT_TYPE_BUFFEREDINPUT:
		(*userFct)(cmd,(void*)callerContext);
		break;
	case CONTEXT_TYPE_NONE:
		(*userFct)(cmd,0);
		break;
	default:
		printf("ERROR: unknown context type in callUserFunction\r\n");
	}
}


void handleCommand(char * cmd,void* caller)
{
	uint8_t cnt = 0;
	uint8_t cmdFound = 0;
	while(userCommands[cnt].commandFct != 0 && cmdFound == 0)
	{
		if (startsWith(cmd,userCommands[cnt].commandName) > 0)
		{
			callUserFunction(userCommands[cnt].commandFct,cmd,userCommands[cnt].contextType,caller);
			cmdFound = 1;
		}
		cnt++;
	}
	if(cmdFound == 0)
	{
		printf("\r\nERROR: Unrecognized Command");
	}
}








