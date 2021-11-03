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

char* onCharacterReception(BufferedInput binput,uint8_t charin)
{
	uint8_t c1=0;
	uint8_t obCnt=0;

    clearOutBuffer(binput->console);
	if (charin == 13 && binput->console->mode == 0) // convert \r into \r\n, print a new console line
	{
		binput->console->outBfr[obCnt++] = 13;
		binput->console->outBfr[obCnt++] = 10;

		while(consolePrefix[c1] != 0)
		{
			binput->console->outBfr[obCnt]=consolePrefix[c1];
			c1++;
			obCnt++;
		}


		// copy the possibly edited command into the first position of the shadow command buffer
		copyCommandBetweenArrays(binput->console->cbfIdx,0,binput->console->commandBuffer,binput->console->commandBufferShadow);

		// push command into history
		for(uint8_t c=COMMAND_HISTORY_SIZE-1;c>0;c--)
		{
			copyCommand(c-1,c,binput->console->commandBufferShadow);
		}

		binput->console->cbfIdx=0;

		handleCommand(binput->console->commandBufferShadow,binput);

		clearCommandBuffer(binput->console,binput->console->cbfIdx,binput->console->commandBufferShadow);

		// copy shadow into edit buffer
		for (uint16_t cc=0;cc<COMMAND_BUFFER_SIZE*COMMAND_HISTORY_SIZE;cc++)
		{
			*(binput->console->commandBuffer+cc) = *(binput->console->commandBufferShadow+cc);
		}

		binput->console->cursor = 0;

	}



	else if ((charin == 0x7F || charin == 0x8) && binput->console->cbfCnt>0 && binput->console->mode == 0 && binput->console->cursor > 0) // DEL/backspace
	{
        if (binput->console->cursor < binput->console->cbfCnt) // within the command
        {
        	uint8_t swap, backcnt=0;
        	swap=binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor];
        	binput->console->outBfr[obCnt++] = 27;
        	binput->console->outBfr[obCnt++] = 91;
        	binput->console->outBfr[obCnt++] = 68;
        	while (swap != 0)
        	{
        		binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor-1] = swap;
        		binput->console->outBfr[obCnt++] = swap;
        		binput->console->cursor++;
        		backcnt++;
        		swap=binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor];
        	}
        	binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor-1]=0;
        	binput->console->outBfr[obCnt++]=' ';
        	binput->console->cursor++;
        	backcnt++;
        	for (uint8_t q=0;q<backcnt;q++)
        	{
        		binput->console->outBfr[obCnt++] = 27;
        		binput->console->outBfr[obCnt++] = 91;
        		binput->console->outBfr[obCnt++] = 68;
        		binput->console->cursor--;
        	}
        	binput->console->cbfCnt--;
        	binput->console->cursor--;
        }
        else // at the right end (default after typing)
        {
        	binput->console->outBfr[obCnt++] = 0x8;
        	binput->console->outBfr[obCnt++] = 0x20;
        	binput->console->outBfr[obCnt++] = 0x8;
        	binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cbfCnt--] = 0;
        	binput->console->cursor--;
        }
	}
	else if (charin < 32) // control character received
	{
		binput->console->mode = 1;
		binput->console->cmdBfr[0]=0;
		binput->console->cmdBfr[1]=0;
		binput->console->cmdBfr[2]=0;
	}
	else if (binput->console->mode==1) // first describing character after the control character
	{
		binput->console->cmdBfr[binput->console->mode-1] = charin;
		binput->console->mode++;
	}
	else if (binput->console->mode==2) 		// command mode, arrows behave rougly the same as in bash
	{
		binput->console->cmdBfr[binput->console->mode-1] = charin;
		binput->console->mode = 0;
		if (strcmp(binput->console->cmdBfr,cmd_arrow_left)==0)
		{
			if (binput->console->cursor>0)
			{
				binput->console->cursor--;
				binput->console->outBfr[obCnt++] = 27;
				binput->console->outBfr[obCnt++] = 91;
				binput->console->outBfr[obCnt++] = 68;
			}
		}
		if (strcmp(binput->console->cmdBfr,cmd_arrow_right)==0)
		{
			if (binput->console->cursor<binput->console->cbfCnt)
			{
				binput->console->cursor++;
				binput->console->outBfr[obCnt++] = 27;
				binput->console->outBfr[obCnt++] = 91;
				binput->console->outBfr[obCnt++] = 67;
			}
		}
		if (strcmp(binput->console->cmdBfr,cmd_arrow_up)==0)
		{
			// remove old command
			while (binput->console->cbfCnt>0)
			{
				binput->console->outBfr[obCnt++] = 0x8;
				binput->console->outBfr[obCnt++] = 0x20;
				binput->console->outBfr[obCnt++] = 0x8;
				binput->console->cbfCnt--;
			}
			if (binput->console->cbfIdx < COMMAND_HISTORY_SIZE-1)
			{
				binput->console->cbfIdx++;
				binput->console->cbfCnt=0;
				binput->console->cursor=0;
				while (binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cbfCnt] != 0)
				{
					binput->console->outBfr[obCnt++] = binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cbfCnt++];
					binput->console->cursor++;
				}
			}
		}
		if (strcmp(binput->console->cmdBfr,cmd_arrow_down)==0)
		{
			// remove old command
			while (binput->console->cbfCnt>0)
			{
				binput->console->outBfr[obCnt++] = 0x8;
				binput->console->outBfr[obCnt++] = 0x20;
				binput->console->outBfr[obCnt++] = 0x8;
				binput->console->cbfCnt--;
			}
			if (binput->console->cbfIdx >0)
			{
				binput->console->cbfIdx--;
				binput->console->cbfCnt=0;
				binput->console->cursor=0;
				while (binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cbfCnt] != 0)
				{
					binput->console->outBfr[obCnt++] = binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cbfCnt++];
					binput->console->cursor++;
				}
			}
		}
	}
	else if (charin < 127) // "normal" (non-control) character nor del or special characters entered
	{
		if (binput->console->cursor < binput->console->cbfCnt) // within the command
		{
			char swap, swap2, backcnt=0;
			swap = binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor];
			binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor++]=charin;
			binput->console->outBfr[obCnt++]=charin;
			while (swap != 0)
			{
				swap2 = binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor];
				binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cursor++]= swap;
				binput->console->outBfr[obCnt++]=swap;
				swap=swap2;
				backcnt++;
			}
           for (uint8_t q =0;q<backcnt;q++)
           {
        	   binput->console->outBfr[obCnt++] = 27;
        	   binput->console->outBfr[obCnt++] = 91;
        	   binput->console->outBfr[obCnt++] = 68;
        	   binput->console->cursor--;
           }
           binput->console->cbfCnt++;
		}
		else
		{
			binput->console->commandBuffer[binput->console->cbfIdx*COMMAND_BUFFER_SIZE + binput->console->cbfCnt++] = charin;
			binput->console->outBfr[obCnt++] = charin;
			binput->console->cursor++;
		}
	}
	binput->console->outBfr[obCnt++] = 0;
	return binput->console->outBfr;
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

