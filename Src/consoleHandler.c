#include "types.h"
#include "consoleHandler.h"
#include <string.h>
#include "taskManager.h"
#include "system.h"

char commandBuffer[COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE];
char commandBufferShadow[COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE];
char outBfr[OUT_BUFFER_SIZE];
char cmdBfr[3];
volatile uint8_t cbfCnt = 0;
volatile uint8_t cbfIdx = 0;
uint8_t cursor;
uint8_t mode = 0; // >0 for handling command code

const char * consolePrefix = "lamp-os>";
const char * cmd_arrow_left = "[D";
const char * cmd_arrow_right = "[C";
const char * cmd_arrow_up = "[A";
const char * cmd_arrow_down = "[B";

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


		// copy the possibly edited command into the first position of the shadow command buffer
		copyCommandBetweenArrays(cbfIdx,0,commandBuffer,commandBufferShadow);

		// push command into history
		for(uint8_t c=COMMAND_HISTORY_SIZE-1;c>0;c--)
		{
			copyCommand(c-1,c,commandBufferShadow);
		}

		cbfIdx=0;

		handleCommand(commandBuffer,lamps);

		clearCommandBuffer(cbfIdx,commandBufferShadow);

		// copy shadow into edit buffer
		for (uint16_t cc=0;cc<COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE;cc++)
		{
			*(commandBuffer+cc) = *(commandBufferShadow+cc);
		}

		cursor = 0;

	}



	else if (c == 0x7F && cbfCnt>0 && mode == 0 && cursor > 0) // DEL/backspace
	{
        if (cursor < cbfCnt) // within the command
        {
        	uint8_t swap, backcnt=0;
        	swap=commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor];
			outBfr[obCnt++] = 27;
			outBfr[obCnt++] = 91;
			outBfr[obCnt++] = 68;
        	while (swap != 0)
        	{
        		commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor-1] = swap;
        		outBfr[obCnt++] = swap;
        		cursor++;
        		backcnt++;
        		swap=commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor];
        	}
        	commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor-1]=0;
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
        	commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cbfCnt--] = 0;
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
	else if (mode==2) 		// command mode, arrows behave rougly the same as in bash
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
		if (strcmp(cmdBfr,cmd_arrow_up)==0)
		{
			// remove old command
			while (cbfCnt>0)
			{
	        	outBfr[obCnt++] = 0x8;
	        	outBfr[obCnt++] = 0x20;
	        	outBfr[obCnt++] = 0x8;
	        	cbfCnt--;
			}
			if (cbfIdx < COMMAND_HISTORY_SIZE-1)
			{
				cbfIdx++;
				cbfCnt=0;
				cursor=0;
				while (commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cbfCnt] != 0)
				{
					outBfr[obCnt++] = commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cbfCnt++];
					cursor++;
				}
			}
		}
		if (strcmp(cmdBfr,cmd_arrow_down)==0)
		{
			// remove old command
			while (cbfCnt>0)
			{
	        	outBfr[obCnt++] = 0x8;
	        	outBfr[obCnt++] = 0x20;
	        	outBfr[obCnt++] = 0x8;
	        	cbfCnt--;
			}
			if (cbfIdx >0)
			{
				cbfIdx--;
				cbfCnt=0;
				cursor=0;
				while (commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cbfCnt] != 0)
				{
					outBfr[obCnt++] = commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cbfCnt++];
					cursor++;
				}
			}
		}
	}
	else if (c < 127) // "normal" (non-control) character nor del or special characters entered
	{
		if (cursor < cbfCnt) // within the command
		{
			char swap, swap2, backcnt=0;
			swap = commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor];
			commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor++]=c;
			outBfr[obCnt++]=c;
			while (swap != 0)
			{
				swap2 = commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor];
				commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cursor++]= swap;
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
			commandBuffer[cbfIdx*COMMAND_BUFFER_SIZE + cbfCnt++] = c;
			outBfr[obCnt++] = c;
			cursor++;
		}
	}
	outBfr[obCnt++] = 0;
	return outBfr;
}


void clearCommandBuffer(uint8_t idx,char* cmdBfr)
{
	for(uint16_t cnt=0; cnt<COMMAND_BUFFER_SIZE; cnt++)
	{
		*(cmdBfr + idx*COMMAND_BUFFER_SIZE + cnt) = 0;
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

void copyCommand(uint8_t idxSrc,uint8_t idxTarget,char* cmdBfr)
{
	for (uint16_t c=0;c<COMMAND_BUFFER_SIZE;c++)
	{
		*(cmdBfr + idxTarget*COMMAND_BUFFER_SIZE + c) = *(cmdBfr + idxSrc*COMMAND_BUFFER_SIZE + c);
	}
}

void copyCommandBetweenArrays(uint8_t idxSrc,uint8_t idxTarget,char* cmdBfrSrc,char* cmdBfrTarget)
{
	for (uint16_t c=0;c<COMMAND_BUFFER_SIZE;c++)
	{
		*(cmdBfrTarget + idxTarget*COMMAND_BUFFER_SIZE + c) = *(cmdBfrSrc + idxSrc*COMMAND_BUFFER_SIZE + c);
	}
}

