#include "types.h"
#include "consoleHandler.h"
#include <string.h>
#include "taskManager.h"
#include "system.h"


void initConsole(Console console)
{
	uint16_t cnt;

	for (cnt=0;cnt<COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE;cnt++)
	{
		console->commandBuffer[cnt]=0;
		console->commandBufferShadow[cnt]=0;
	}
	for (cnt=0;cnt<OUT_BUFFER_SIZE;cnt++)
	{
		console->outBfr[cnt]=0;
	}
	console->cmdBfr[0]=0;
	console->cmdBfr[1]=0;
	console->cmdBfr[2]=0;
	console->cbfCnt=0;
	console->cbfIdx=0;
	console->cursor=0;
	console->mode=0;
}

const char * consolePrefix = "lamp-os>";
const char * cmd_arrow_left = "[D";
const char * cmd_arrow_right = "[C";
const char * cmd_arrow_up = "[A";
const char * cmd_arrow_down = "[B";

char* onCharacterReception(Console console,uint8_t c,RGBStream * lamps)
{
	uint8_t c1=0;
	uint8_t obCnt=0;

    clearOutBuffer(console);
	if (c == 13 && console->mode == 0) // convert \r into \r\n, print a new console line
	{
		console->outBfr[obCnt++] = 13;
		console->outBfr[obCnt++] = 10;

		while(consolePrefix[c1] != 0)
		{
			console->outBfr[obCnt]=consolePrefix[c1];
			c1++;
			obCnt++;
		}


		// copy the possibly edited command into the first position of the shadow command buffer
		copyCommandBetweenArrays(console->cbfIdx,0,console->commandBuffer,console->commandBufferShadow);

		// push command into history
		for(uint8_t c=COMMAND_HISTORY_SIZE-1;c>0;c--)
		{
			copyCommand(c-1,c,console->commandBufferShadow);
		}

		console->cbfIdx=0;

		handleCommand(console->commandBufferShadow);

		clearCommandBuffer(console,console->cbfIdx,console->commandBufferShadow);

		// copy shadow into edit buffer
		for (uint16_t cc=0;cc<COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE;cc++)
		{
			*(console->commandBuffer+cc) = *(console->commandBufferShadow+cc);
		}

		console->cursor = 0;

	}



	else if ((c == 0x7F || c == 0x8) && console->cbfCnt>0 && console->mode == 0 && console->cursor > 0) // DEL/backspace
	{
        if (console->cursor < console->cbfCnt) // within the command
        {
        	uint8_t swap, backcnt=0;
        	swap=console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor];
        	console->outBfr[obCnt++] = 27;
        	console->outBfr[obCnt++] = 91;
        	console->outBfr[obCnt++] = 68;
        	while (swap != 0)
        	{
        		console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor-1] = swap;
        		console->outBfr[obCnt++] = swap;
        		console->cursor++;
        		backcnt++;
        		swap=console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor];
        	}
        	console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor-1]=0;
        	console->outBfr[obCnt++]=' ';
        	console->cursor++;
        	backcnt++;
        	for (uint8_t q=0;q<backcnt;q++)
        	{
        		console->outBfr[obCnt++] = 27;
        		console->outBfr[obCnt++] = 91;
        		console->outBfr[obCnt++] = 68;
        		console->cursor--;
        	}
        	console->cbfCnt--;
        	console->cursor--;
        }
        else // at the right end (default after typing)
        {
        	console->outBfr[obCnt++] = 0x8;
        	console->outBfr[obCnt++] = 0x20;
        	console->outBfr[obCnt++] = 0x8;
        	console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cbfCnt--] = 0;
        	console->cursor--;
        }
	}
	else if (c < 32) // control character received
	{
		console->mode = 1;
		console->cmdBfr[0]=0;
		console->cmdBfr[1]=0;
		console->cmdBfr[2]=0;
	}
	else if (console->mode==1) // first describing character after the control character
	{
		console->cmdBfr[console->mode-1] = c;
		console->mode++;
	}
	else if (console->mode==2) 		// command mode, arrows behave rougly the same as in bash
	{
		console->cmdBfr[console->mode-1] = c;
		console->mode = 0;
		if (strcmp(console->cmdBfr,cmd_arrow_left)==0)
		{
			if (console->cursor>0)
			{
				console->cursor--;
				console->outBfr[obCnt++] = 27;
				console->outBfr[obCnt++] = 91;
				console->outBfr[obCnt++] = 68;
			}
		}
		if (strcmp(console->cmdBfr,cmd_arrow_right)==0)
		{
			if (console->cursor<console->cbfCnt)
			{
				console->cursor++;
				console->outBfr[obCnt++] = 27;
				console->outBfr[obCnt++] = 91;
				console->outBfr[obCnt++] = 67;
			}
		}
		if (strcmp(console->cmdBfr,cmd_arrow_up)==0)
		{
			// remove old command
			while (console->cbfCnt>0)
			{
				console->outBfr[obCnt++] = 0x8;
				console->outBfr[obCnt++] = 0x20;
				console->outBfr[obCnt++] = 0x8;
				console->cbfCnt--;
			}
			if (console->cbfIdx < COMMAND_HISTORY_SIZE-1)
			{
				console->cbfIdx++;
				console->cbfCnt=0;
				console->cursor=0;
				while (console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cbfCnt] != 0)
				{
					console->outBfr[obCnt++] = console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cbfCnt++];
					console->cursor++;
				}
			}
		}
		if (strcmp(console->cmdBfr,cmd_arrow_down)==0)
		{
			// remove old command
			while (console->cbfCnt>0)
			{
				console->outBfr[obCnt++] = 0x8;
				console->outBfr[obCnt++] = 0x20;
				console->outBfr[obCnt++] = 0x8;
				console->cbfCnt--;
			}
			if (console->cbfIdx >0)
			{
				console->cbfIdx--;
				console->cbfCnt=0;
				console->cursor=0;
				while (console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cbfCnt] != 0)
				{
					console->outBfr[obCnt++] = console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cbfCnt++];
					console->cursor++;
				}
			}
		}
	}
	else if (c < 127) // "normal" (non-control) character nor del or special characters entered
	{
		if (console->cursor < console->cbfCnt) // within the command
		{
			char swap, swap2, backcnt=0;
			swap = console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor];
			console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor++]=c;
			console->outBfr[obCnt++]=c;
			while (swap != 0)
			{
				swap2 = console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor];
				console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cursor++]= swap;
				console->outBfr[obCnt++]=swap;
				swap=swap2;
				backcnt++;
			}
           for (uint8_t q =0;q<backcnt;q++)
           {
        	   console->outBfr[obCnt++] = 27;
        	   console->outBfr[obCnt++] = 91;
        	   console->outBfr[obCnt++] = 68;
        	   console->cursor--;
           }
           console->cbfCnt++;
		}
		else
		{
			console->commandBuffer[console->cbfIdx*COMMAND_BUFFER_SIZE + console->cbfCnt++] = c;
			console->outBfr[obCnt++] = c;
			console->cursor++;
		}
	}
	console->outBfr[obCnt++] = 0;
	return console->outBfr;
}


void clearCommandBuffer(Console console,uint8_t idx,char* cmdBfr)
{
	for(uint16_t cnt=0; cnt<COMMAND_BUFFER_SIZE; cnt++)
	{
		*(cmdBfr + idx*COMMAND_BUFFER_SIZE + cnt) = 0;
	}
	console->cbfCnt=0;
}

void clearOutBuffer(Console console)
{
	for(uint16_t cnt=0; cnt<OUT_BUFFER_SIZE; cnt++)
	{
		console->outBfr[cnt] = 0;
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

