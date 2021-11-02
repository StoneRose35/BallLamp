/*
 * apiHandler.c
 *
 *  Created on: 01.11.2021
 *      Author: philipp
 */

#include "system.h"
#include "apiHandler.h"
#include "taskManager.h"


void initApi(Api api)
{
	api->ibidx = 0;
	for(uint16_t c=0;c<API_INPUT_BUFFER_SIZE;c++)
	{
		api->inputBuffer[c] = 0;
	}
}

char* onByteReception(Api api,uint8_t charin)
{
	if (charin == 13) // handle command when a newline character has been received
	{
		handleCommand(api->inputBuffer);
	}
	else
	{
		api->inputBuffer[api->ibidx++] = charin;
		if (api->ibidx > API_INPUT_BUFFER_SIZE)
		{
			api->outputBuffer[0] = 0x31;
		}
	}
	api->outputBuffer[0] = 0x30;
	return api->outputBuffer;
}

