/**
 * @file taskManager.c
 * @author Philipp Fuerholz (fuerholz@gmx.ch)
 * @brief currently contains all user-callable functions, related helper functions and data structures and the 
 * task Manager functions themselves.
 * 
 * All user-callable functions should print error messages to indicate exceptions. User functions should not access hardware buffers
 * directly and be system or mcu independent.
 * @version 0.1
 * @date 2021-12-23
 * 
 * 
 */
#include "systemChoice.h"
#include "taskManager.h"
#include "system.h"
#include "memoryAccess.h"
#include "flash.h"
#ifdef HARDWARE
#include "uart.h"
#include "systick.h"
register void* address __asm__("sp");
#else
#include <stdio.h>
void* address;
#endif
#include "stringFunctions.h"
#include "interpolators.h"
#include "taskManagerUtils.h"
#include <bufferedInputHandler.h>
#include "bluetoothATConfig.h"

#include <string.h>
#include <stdlib.h>
#include <malloc.h>

/**
 * @brief array of 16 standard colors used for the color command
 * 
 */
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

/**
 * @brief command name of the color commands
 * 
 */
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

/**
 * @brief implementation of the command which sets the color of a set of neopixel colors to a preset color
 * 
 * @param cmd the command itself, Example AQUA(0-4,10)
 * @param context the RGBStream containing the structured color information
 */
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

/**
 * @brief set a set of neopixel to a custom rgb value
 * @param cmd The command including argument, Example RGB(110,0,110,1,3,5-7) set the neopixels 1,3,5,6,7
 * to a dim violet.
 * @param context the RGBStream containing the structured color information
 */
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


/**
 * @brief Starts the neopixel animation, if possible
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the Tasks objects
 */
void startCommand(char * cmd,void* context)
{
	uint8_t retcode;
	Tasks interpolators=(Tasks)context;
	retcode = startInterpolators(interpolators);
	if (retcode > 0)
	{
		printf("Error while starting the animation\r\n");
	}
}

/**
 * @brief Stops the neopixel animation, if possible
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the Tasks objects
 */
void stopCommand(char * cmd,void* context)
{
	uint8_t retcode;
	Tasks interpolators=(Tasks)context;
	retcode = stopInterpolators(interpolators);
	if (retcode > 0)
	{
		printf("Error while stopping the animation\r\n");
	}
}

/**
 * @brief prints a brief overview of the commands
 * 
 * @param cmd the command, doesn't take any arguments
 * @param context unused
 */
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
	printf(" * SYSINFO: displays system and memory infos\r\n");

}

/**
 * @brief defines an interpolator/color sequence for a given neopixel 255 means every neopixel. This function doesn't take ranges
 * as an argument to select a set of neopixels.
 * 
 * @param cmd the command itself including arguments.
 * @param context the Tasks data structures holding all individual neopixel Task data structures 
 */
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

/**
 * @brief defines a certain step for a color sequence/interpolator
 * 
 * @param cmd the command itself including arguments.
 * @param context the Tasks data structures holding all individual neopixel Task data structures 
 */
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

/**
 * @brief shows the state of all color sequences including their progression
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the Tasks data structure
 */
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

/**
 * @brief frees a color sequence / interpolator data structures
 * 
 * returns error messages if no destroyable interpolators have been found
 * @param cmd the command including arguments.
 * @param context the Tasks data structure.
 */
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

/**
 * @brief persistently stores a set of interpolators / color sequences
 * return a message indicating how many bytes have been saved and is saving has been successful.
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the Tasks data structure.
 */
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

/**
 * @brief load a set of interpolators from the persistent storage
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the Tasks data structure.
 */
void loadCommand(char * cmd,void* context)
{
	Tasks interpolators=(Tasks)context;
	for (uint8_t c=0;c<interpolators->taskArrayLength;c++)
	{
		destroyTask(interpolators->taskArray+c);
	}
	fromStream((uint16_t*)((ptr)getFilesystemStart()+FLASH_HEADER_SIZE),interpolators);
}

/**
 * @brief switches the mode of the interface to API. Nothing happens if the mode is already API.
 * 
 * Note: it is only possible to change the mode of the interface which is accessed.
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the BufferedInput from which the command has been sent.
 */
void apiCommand(char * cmd,void* context)
{
	BufferedInput binput = (BufferedInput)context;
	binput->interfaceType = BINPUT_TYPE_API;
	printf("APIOK\n");
	binput->console->outBfr[0]=0; // disable command prompt already generated by the console handler
}

/**
 * @brief switches the mode of the interface to Console/CLI. Nothing happens if the mode is already Console/CLI.
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the BufferedInput from which the command has been sent.
 */
void consoleCommand(char * cmd,void* context)
{
	BufferedInput binput = (BufferedInput)context;
	binput->interfaceType = BINPUT_TYPE_CONSOLE;
	printf("Switching back to Console Mode\r\n");
}

/**
 * @brief sets up Bluetooth, Deprecated.
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context unused
 */
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

/**
 * @brief prints out system information
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context unused
 */
void sysInfoCommand(char * cmd,void*context)
{
	uint32_t current_sp = (uint32_t)address;
	struct mallinfo heapInfo;
	uint32_t nticks;
	char nrbfr[16];
	heapInfo = mallinfo();
	nticks = getTickValue();
	printf("\r\n\r\nTicks since Start: ");
	UInt32ToChar(nticks,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Current Stack Pointer Location (bytes): ");
	UInt32ToHex(current_sp,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Total Allocated Space (bytes): ");
	UInt32ToChar((uint32_t)heapInfo.uordblks,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Total Free Space (bytes): ");
	UInt32ToChar((uint32_t)heapInfo.fordblks,nrbfr);
	printf(nrbfr);
	printf("\r\n");
}

/**
 * @brief the currently implemented commands
 * The "0" command is used to mark the end of the array, it is not an actual command
 * 
 */
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
	{"SYSINFO",&sysInfoCommand,CONTEXT_TYPE_NONE},
	{"HELP",&helpCommand,CONTEXT_TYPE_NONE},
	{"0",0}
};


/**
 * @brief sets the appropriate system context and calls the user functions
 * 
 * A system context can either be the global RGBStream defining the structured color information for all neopixels or the TasksType holding the
 * animation for the neopixels.
 * @param userFct pointer to the function which should be executed
 * @param cmd the command including possible arguments
 * @param contextType the context type enumeration, depending on this value a system context or null is handed over
 * @param callerContext an optional caller context, currently used for API and CONSOLE
 */
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

/**
 * @brief The actual task manager
 * 
 * hands the command over to callUserFunction if the command name has been found.
 * @param cmd the commmand including possible arguments
 * @param caller generic context, currently for API and CONSOLE the calling BufferedInput is handed over, is null for all other commands
 */
void handleCommand(char * cmd,void* caller)
{
	uint8_t cnt = 0;
	uint8_t cmdFound = 0;
	if (*cmd!= 0)
	{
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
}









