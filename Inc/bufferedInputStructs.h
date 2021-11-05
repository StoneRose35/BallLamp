/*
 * bufferedInputStructs.h
 *
 *  Created on: 03.11.2021
 *      Author: philipp
 */

#ifndef BUFFEREDINPUTSTRUCTS_H_
#define BUFFEREDINPUTSTRUCTS_H_

#ifndef STM32
#include <stdint.h>
#endif

#define COMMAND_BUFFER_SIZE 48
#define COMMAND_HISTORY_SIZE 4
#define OUT_BUFFER_SIZE 256

#define API_INPUT_BUFFER_SIZE 64
#define API_OUTPUT_BUFFER_SIZE 8

#define INPUT_BUFFER_SIZE 8
#define OUTPUT_BUFFER_SIZE 256

typedef struct {

	char inputBuffer[API_INPUT_BUFFER_SIZE];
	char outputBuffer[API_OUTPUT_BUFFER_SIZE];
	uint16_t ibidx;
} ApiType;


typedef ApiType* Api;


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

#endif /* BUFFEREDINPUTSTRUCTS_H_ */
