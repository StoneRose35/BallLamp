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

#define N_COMMANDS 6
#define TASK_STATE_STOPPED 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_PAUSED 2


void handleCommand(const char*,RGBStream*);

void handleOff(uint8_t,RGBStream*);
void handleWhite(uint8_t,RGBStream*);
void handleRed(uint8_t,RGBStream*);
void handleBlue(uint8_t,RGBStream*);
void handleGreen(uint8_t,RGBStream*);
void handleHelp(uint8_t,RGBStream*);
void handleRgb(uint8_t,uint8_t,uint8_t,uint8_t,RGBStream*);


#endif /* TASKMANAGER_H_ */
