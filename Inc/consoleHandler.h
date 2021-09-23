/*
 * consoleHandler.h
 *
 *  Created on: 15.09.2021
 *      Author: philipp
 */
#include "system.h"

#ifndef CONSOLEHANDLER_H_
#define CONSOLEHANDLER_H_

#define COMMAND_BUFFER_SIZE 128
#define COMMAND_HISTORY_SIZE 4
#define OUT_BUFFER_SIZE 256

char* onCharacterReception(uint8_t,RGBStream * lamps);
void clearCommandBuffer(uint8_t);
void clearOutBuffer();
void copyCommand(uint8_t,uint8_t); // copies a command from index src to index target


#endif /* CONSOLEHANDLER_H_ */
