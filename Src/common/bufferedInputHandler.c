/*
 * byteInterfaceHandler.c
 *
 *  Created on: 02.11.2021
 *      Author: philipp
 */

#include "bufferedInputHandler.h"
#include "consoleHandler.h"
#include "apiHandler.h"
#ifndef STM32
#include <stdio.h>
#endif

void processInputBuffer(BufferedInput binput)
{
	char* returnBfr;
	if (binput->interfaceType == BINPUT_TYPE_CONSOLE)
	{
		while(binput->commBuffer->inputBufferCnt > 0)
		{
			returnBfr = onCharacterReception(binput,binput->commBuffer->inputBuffer[binput->commBuffer->inputBufferCnt-1]);
			binput->commBuffer->inputBufferCnt--;
			printf(returnBfr);
		}

	} else if (binput->interfaceType == BINPUT_TYPE_API)
	{
		while(binput->commBuffer->inputBufferCnt > 0)
		{
			returnBfr = onByteReception(binput,binput->commBuffer->inputBuffer[binput->commBuffer->inputBufferCnt-1]);
			binput->commBuffer->inputBufferCnt--;
			printf(returnBfr);
		}
	}

}