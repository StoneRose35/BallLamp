/*
 * consoleHandler.h
 *
 *  Created on: 15.09.2021
 *      Author: philipp
 */
#include "system.h"

#ifndef CONSOLEHANDLER_H_
#define CONSOLEHANDLER_H_

#define COMMAND_BUFFER_SIZE 32
#define COMMAND_HISTORY_SIZE 4
#define OUT_BUFFER_SIZE 256

char* onCharacterReception(uint8_t,RGBStream * lamps);
void clearCommandBuffer(uint8_t,char*);
void clearOutBuffer();
void copyCommand(uint8_t,uint8_t,char*); // copies a command from index src to index target
void copyCommandBetweenArrays(uint8_t,uint8_t,char*,char*);

#endif /* CONSOLEHANDLER_H_ */
