/*
 * apiHandler.c
 *
 *  Created on: 01.11.2021
 *      Author: philipp
 */

#include "system.h"
#include "apiHandler.h"
#include "taskManager.h"
#include "bufferedInputHandler.h"


void initApi(Api api)
{
	api->ibidx = 0;
	for(uint16_t c=0;c<API_INPUT_BUFFER_SIZE;c++)
	{
		api->inputBuffer[c] = 0;
	}
}

char* onByteReception(BufferedInput binput,uint8_t charin)
{
	if (charin == 13) // handle command when a newline character has been received
	{
		handleCommand(binput->api->inputBuffer,binput);

		binput->api->ibidx = 0;
		for(uint16_t c=0;c<API_INPUT_BUFFER_SIZE;c++)
		{
			binput->api->inputBuffer[c] = 0;
		}
		binput->api->outputBuffer[0] = 0;
		binput->api->outputBuffer[1] = 0;
	}
	else
	{
		binput->api->inputBuffer[binput->api->ibidx++] = charin;
		if (binput->api->ibidx >= API_INPUT_BUFFER_SIZE)
		{
			binput->api->ibidx = API_INPUT_BUFFER_SIZE-1;
			binput->api->outputBuffer[0] = 0x31;
		}
		else
		{
			binput->api->outputBuffer[0]=0;
		}
	}
	binput->api->outputBuffer[1] = 0;
	return binput->api->outputBuffer;
}

