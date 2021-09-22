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
#define OUT_BUFFER_SIZE 256

char* onCharacterReception(uint8_t,RGBStream * lamps);
void clearCommandBuffer();
void clearOutBuffer();


#endif /* CONSOLEHANDLER_H_ */
