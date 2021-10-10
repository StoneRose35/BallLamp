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

#define N_COMMANDS 17
#define TASK_STATE_STOPPED 0
#define TASK_STATE_RUNNING 1
#define TASK_STATE_PAUSED 2


void handleCommand(char*,RGBStream*);


void handleBackground(uint8_t,RGBStream*);
void handleForground(uint8_t,RGBStream*);
void handleRed(uint8_t,RGBStream*);
void handleGreen(uint8_t,RGBStream*);
void handleDarkblue(uint8_t,RGBStream*);
void handleLightblue(uint8_t,RGBStream*);
void handleMagenta(uint8_t,RGBStream*);
void handleYellow(uint8_t,RGBStream*);
void handleOrange(uint8_t,RGBStream*);
void handlePurple(uint8_t,RGBStream*);
void handleYellowgreen(uint8_t,RGBStream*);
void handleMediumblue(uint8_t,RGBStream*);
void handleDarkyellow(uint8_t,RGBStream*);
void handleAqua(uint8_t,RGBStream*);
void handleDarkpurple(uint8_t,RGBStream*);
void handleGray(uint8_t,RGBStream*);

void handleHelp(uint8_t,RGBStream*);

void handleRgb(uint8_t,uint8_t,uint8_t,uint8_t,RGBStream*);


#endif /* TASKMANAGER_H_ */
