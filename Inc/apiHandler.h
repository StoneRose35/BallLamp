/*
 * apiHandler.h
 *
 *  Created on: 01.11.2021
 *      Author: philipp
 */
#include "system.h"

#ifndef APIHANDLER_H_
#define APIHANDLER_H_

#define API_INPUT_BUFFER_SIZE 64
#define API_OUTPUT_BUFFER_SIZE 8

typedef struct {

	char inputBuffer[API_INPUT_BUFFER_SIZE];
	char outputBuffer[API_OUTPUT_BUFFER_SIZE];
	//char commandBuffer[COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE];
	//char commandBufferShadow[COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE];
	//char outBfr[OUT_BUFFER_SIZE];
	//char cmdBfr[3];
	//uint8_t cbfCnt;
	//uint8_t cbfIdx;
	//uint8_t cursor;
	//uint8_t mode;
	uint16_t ibidx;
} ApiType;

typedef ApiType* Api;


void initApi(Api api);
char* onByteReception(Api console,uint8_t c);

#endif /* APIHANDLER_H_ */
