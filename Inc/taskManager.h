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

void handleCommand(const char*);

void handleOff(uint8_t);
void handleWhite(uint8_t);
void handleRed(uint8_t);
void handleBlue(uint8_t);
void handleGreen(uint8_t);
void handleHelp(uint8_t);

#endif /* TASKMANAGER_H_ */
