/*
 * taskManager.h
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include "types.h"

#ifndef TASKMANAGER_H_
#define TASKMANAGER_H_

#define N_COMMANDS 6
#define TASK_STATE_STOPPED 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_PAUSED 2

typedef struct {
	void(*updateFunction)(void*);
	uint8_t state;
	uint8_t lamp_nr;
} TaskType;

void handleCommand(const char*);

void handleOff(uint8_t);
void handleWhite(uint8_t);
void handleRed(uint8_t);
void handleBlue(uint8_t);
void handleGreen(uint8_t);
void handleHelp(uint8_t);
void handleRgb(uint8_t,uint8_t,uint8_t,uint8_t);


#endif /* TASKMANAGER_H_ */
