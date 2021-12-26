/*
 * taskManager.h
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include <stdint.h>
#include "system.h"

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#define N_COLOR_COMMANDS 16 //!< the number of commands which set all neopixels to the same color
#define TASK_STATE_STOPPED 0 //!< enum value: the command is stopped
#define TASK_STATE_RUNNING 1 //!< enum value: the command is running
#define TASK_STATE_PAUSED 2 //!< enum value: the command is paused

#define CONTEXT_TYPE_NONE 0 //!< the second argument passed to commandFct in is a null pointer
#define CONTEXT_TYPE_RGBSTREAM 1 //!< the second argment passed to commandFct is a RgbStream
#define CONTEXT_TYPE_INTERPOLATORS 2 //!< the second argment passed to commandFct is a Tasks data structure
#define CONTEXT_TYPE_BUFFEREDINPUT 3 //!< the second argment passed to commandFct is a BufferedInput
/**
 * @brief compound of a defineable command name und a function pointer
 * A user command appears as a function in the "fake-bash" command line. The name and command line arguments are passed as char 
 * array into the function pointer. A generic context object can ben provided as second argument to the command function.
 * Furthermore to facilitate typecasting a context type can be defined.
 */
typedef struct
{
	char commandName[16]; //!< name of the command, must contain only printable characters
	void (*commandFct)(char*,void*context); //!< pointer to a function which is executed when the command is entered
	uint8_t contextType; //!< flag defining the context type
} UserCommandType;

void handleCommand(char*,void*);


#endif /* TASKMANAGER_H_ */
