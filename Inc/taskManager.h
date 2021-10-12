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

#define N_COMMANDS 16
#define TASK_STATE_STOPPED 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_PAUSED 2


void handleCommand(char*,RGBStream*);

void handleHelp(uint8_t,RGBStream*);

void handleRgb(uint8_t,uint8_t,uint8_t,uint8_t,RGBStream*);
void handleRgbStruct(RGB,uint8_t,RGBStream*);

/*
 * expands a string list of lamp number into a array of unique lamp numbers
 * */
uint8_t expandLampDescription(char *,uint8_t**);


#endif /* TASKMANAGER_H_ */
