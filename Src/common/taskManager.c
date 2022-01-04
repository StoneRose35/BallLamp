/**
 * @file taskManager.c
 * @author Philipp Fuerholz (fuerholz@gmx.ch)
 * @brief the framework for calling a function from the command line. Contains a hard-coded registers of all user-commands and helper functions to convert the command line input 
 * to a function call.
 * 
 * All user-callable functions should print error messages to indicate exceptions. User functions should not access hardware buffers
 * directly and be system or mcu independent.
 * @version 0.1
 * @date 2021-12-23
 * 
 * 
 */
#include "taskManager.h"

#include "interpolators.h"
#include "stringFunctions.h"
#include "neopixelCommands.h"
#include "utilCommands.h"
#include "sdAccessCommands.h"
#include "displayCommands.h"
#include "uart.h"


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
	{"NPENGINE",&npEngineCommand,CONTEXT_TYPE_NONE},
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
	{"SDINIT",&sdInitCommand,CONTEXT_TYPE_NONE},
	{"MOUNT",&mountCommand, CONTEXT_TYPE_NONE},
	{"INITDISPLAY",&initDisplayCommand,CONTEXT_TYPE_NONE},
	{"BACKLIGHT",&setBacklightCommand,CONTEXT_TYPE_NONE},
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
			if(*((uint8_t*)cmd)>=97 && *((uint8_t*)cmd)<=122)
			{
				toUpper(cmd);
			}
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









