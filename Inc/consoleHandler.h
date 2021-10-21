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

typedef struct {
	char commandBuffer[COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE];
	char commandBufferShadow[COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE];
	char outBfr[OUT_BUFFER_SIZE];
	char cmdBfr[3];
	uint8_t cbfCnt;
	uint8_t cbfIdx;
	uint8_t cursor;
	uint8_t mode;
} ConsoleType;

typedef ConsoleType* Console;

void initConsole(Console);
char* onCharacterReception(Console,uint8_t,RGBStream * lamps);
void clearCommandBuffer(Console,uint8_t,char*);
void clearOutBuffer(Console);
void copyCommand(uint8_t,uint8_t,char*); // copies a command from index src to index target
void copyCommandBetweenArrays(uint8_t,uint8_t,char*,char*);

#endif /* CONSOLEHANDLER_H_ */
