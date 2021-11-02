/*
 * byteInterfaceHandler.h
 *
 *  Created on: 02.11.2021
 *      Author: philipp
 */

#ifndef BUFFEREDINPUTHANDLER_H_
#define BUFFEREDINPUTHANDLER_H_

#include "types.h"
#include "consoleHandler.h"
#include "apiHandler.h"

#include "uart.h"

#define INPUT_BUFFER_SIZE 8
#define OUTPUT_BUFFER_SIZE 256


#define BINPUT_TYPE_CONSOLE 0
#define BINPUT_TYPE_API 1

typedef struct
{
	char inputBuffer[INPUT_BUFFER_SIZE];
	char outputBuffer[OUTPUT_BUFFER_SIZE];
	uint32_t outputBufferReadCnt;
	uint32_t outputBufferWriteCnt;
	uint8_t inputBufferCnt;
} CommBufferType;

typedef CommBufferType* CommBuffer;

typedef struct
{
	uint8_t interfaceType;
	CommBuffer commBuffer;
	Console console;
	Api api;
} BufferedInputType;

typedef BufferedInputType* BufferedInput;

void processInputBuffer(BufferedInput);

#endif /* BUFFEREDINPUTHANDLER_H_ */
