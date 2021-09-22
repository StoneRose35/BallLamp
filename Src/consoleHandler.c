#include "types.h"
#include "consoleHandler.h"
#include <string.h>
#include "taskManager.h"
#include "system.h"

uint8_t commandBuffer[COMMAND_BUFFER_SIZE];
char outBfr[OUT_BUFFER_SIZE];
char cmdBfr[3];
volatile uint8_t cbfCnt;
uint8_t cursor;
uint8_t mode = 0; // >0 for handling command code

const char * consolePrefix = "lamp-os>";
const char * cmd_arrow_left = "[D";
const char * cmd_arrow_right = "[C";

char* onCharacterReception(uint8_t c,RGBStream * lamps)
{
	uint8_t c1=0;
	uint8_t obCnt=0;

    clearOutBuffer();
	if (c == 13 && mode == 0) // convert \r into \r\n, print a new console line
	{
		outBfr[obCnt++] = 13;
		outBfr[obCnt++] = 10;

		while(consolePrefix[c1] != 0)
		{
			outBfr[obCnt]=consolePrefix[c1];
			c1++;
			obCnt++;
		}

		handleCommand((const char*)commandBuffer,lamps);

		clearCommandBuffer();
		cursor = 0;

	}



	else if (c == 0x7F && cbfCnt>0 && mode == 0 && cursor > 0) // DEL/backspace
	{
        if (cursor < cbfCnt) // within the command
        {
        	uint8_t swap, backcnt=0;
        	swap=commandBuffer[cursor];
			outBfr[obCnt++] = 27;
			outBfr[obCnt++] = 91;
			outBfr[obCnt++] = 68;
        	while (swap != 0)
        	{
        		commandBuffer[cursor-1] = swap;
        		outBfr[obCnt++] = swap;
        		cursor++;
        		backcnt++;
        		swap=commandBuffer[cursor];
        	}
        	commandBuffer[cursor-1]=0;
        	outBfr[obCnt++]=' ';
        	cursor++;
        	backcnt++;
        	for (uint8_t q=0;q<backcnt;q++)
        	{
    			outBfr[obCnt++] = 27;
    			outBfr[obCnt++] = 91;
    			outBfr[obCnt++] = 68;
    			cursor--;
        	}
        	cbfCnt--;
        	cursor--;
        }
        else // at the right end (default after typing)
        {
        	outBfr[obCnt++] = 0x8;
        	outBfr[obCnt++] = 0x20;
        	outBfr[obCnt++] = 0x8;
        	commandBuffer[cbfCnt--] = 0;
        	cursor--;
        }
	}
	else if (c < 32) // control character received
	{
		mode = 1;
		cmdBfr[0]=0;
		cmdBfr[1]=0;
		cmdBfr[2]=0;
	}
	else if (mode==1) // first describing character after the control character
	{
		cmdBfr[mode-1] = c;
		mode++;
	}
	else if (mode==2) 		// command mode, so far only arrow left and arrow right is supported
	{
		cmdBfr[mode-1] = c;
		mode = 0;
		if (strcmp(cmdBfr,cmd_arrow_left)==0)
		{
			if (cursor>0)
			{
				cursor--;
				outBfr[obCnt++] = 27;
				outBfr[obCnt++] = 91;
				outBfr[obCnt++] = 68;
			}
		}
		if (strcmp(cmdBfr,cmd_arrow_right)==0)
		{
			if (cursor<cbfCnt)
			{
				cursor++;
				outBfr[obCnt++] = 27;
				outBfr[obCnt++] = 91;
				outBfr[obCnt++] = 67;
			}
		}
	}
	else if (c < 127) // "normal" (non-control) character nor del or special characters entered
	{
		if (cursor < cbfCnt) // within the command
		{
			char swap, swap2, backcnt=0;
			swap = commandBuffer[cursor];
			commandBuffer[cursor++]=c;
			outBfr[obCnt++]=c;
			while (swap != 0)
			{
				swap2 = commandBuffer[cursor];
				commandBuffer[cursor++]= swap;
				outBfr[obCnt++]=swap;
				swap=swap2;
				backcnt++;
			}
           for (uint8_t q =0;q<backcnt;q++)
           {
        	   outBfr[obCnt++] = 27;
        	   outBfr[obCnt++] = 91;
        	   outBfr[obCnt++] = 68;
        	   cursor--;
           }
           cbfCnt++;
		}
		else
		{
			commandBuffer[cbfCnt++] = c;
			outBfr[obCnt++] = c;
			cursor++;
		}
	}
	outBfr[obCnt++] = 0;
	return outBfr;
}


void clearCommandBuffer()
{
	for(uint16_t cnt=0; cnt<COMMAND_BUFFER_SIZE; cnt++)
	{
		commandBuffer[cnt] = 0;
	}
	cbfCnt=0;
}

void clearOutBuffer()
{
	for(uint16_t cnt=0; cnt<OUT_BUFFER_SIZE; cnt++)
	{
		outBfr[cnt] = 0;
	}
}




