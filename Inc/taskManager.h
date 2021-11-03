/*
 * taskManager.h
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include "types.h"
#include "system.h"

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#define N_COLOR_COMMANDS 16
#define TASK_STATE_STOPPED 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_PAUSED 2

#define CONTEXT_TYPE_NONE 0
#define CONTEXT_TYPE_RGBSTREAM 1
#define CONTEXT_TYPE_INTERPOLATORS 2
#define CONTEXT_TYPE_BUFFEREDINPUT 3

typedef struct
{
	char commandName[16];
	void (*commandFct)(char*,void*context);
	uint8_t contextType;
} UserCommandType;

void handleCommand(char*,void*);


#endif /* TASKMANAGER_H_ */
