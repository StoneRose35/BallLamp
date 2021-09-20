/*
 * taskManager.c
 *
 *  Created on: 16.09.2021
 *      Author: philipp
 */

#include "taskManager.h"
#include "neopixelDriver.h"
#include "uart.h"
#include <string.h>
#include <stdlib.h>



extern RGBStream * frame;
const char * known_commands[N_COMMANDS] = {"OFF","WHITE","RED","BLUE","GREEN","--help"};
const void(*functions[])(uint8_t) ={handleOff, handleWhite, handleRed, handleBlue, handleGreen, handleHelp};

void (*handleOffPtr)(uint8_t) = &handleOff;

void handleCommand(const char * cmd)
{
	uint8_t cmdFound = 0;
	char nrbfr[4];
	if (*cmd != 0)
	{
		for(uint8_t cnt=0;cnt<N_COMMANDS;cnt++)
		{
			if (startsWith(cmd,known_commands[cnt])>0)
			{
				cmdFound=1;
				if (functions[cnt] != 0)
				{
					getBracketContent(cmd,nrbfr);
					if (nrbfr != 0)
					{
						(*functions[cnt])(toInt(nrbfr));
					}
				}
				else
				{
					printf("\r\nNotImplementedException");
				}
			}
		}
		if (cmdFound == 0)
		{
			printf("\r\nUnrecognizedCommandException");
			handleHelp(0);
		}
	}

}

void handleOff(uint8_t nr)
{
	(frame+nr)->rgb.r=0;
	(frame+nr)->rgb.b=0;
	(frame+nr)->rgb.g=0;
}

void handleWhite(uint8_t nr)
{
	(frame+nr)->rgb.r=0xFF;
	(frame+nr)->rgb.b=0xFF;
	(frame+nr)->rgb.g=0xFF;
}

void handleRed(uint8_t nr)
{
	(frame+nr)->rgb.r=0xFF;
	(frame+nr)->rgb.b=0x00;
	(frame+nr)->rgb.g=0x00;
}

void handleGreen(uint8_t nr)
{
	(frame+nr)->rgb.r=0x00;
	(frame+nr)->rgb.b=0x00;
	(frame+nr)->rgb.g=0xFF;
}

void handleBlue(uint8_t nr)
{
	(frame+nr)->rgb.r=0x00;
	(frame+nr)->rgb.b=0xFF;
	(frame+nr)->rgb.g=0x00;
}
void handleHelp(uint8_t nr)
{
	char nrbfr[4];
	printf("\r\nSupported commands are\r\n");
	for(uint8_t c;c<N_COMMANDS;c++)
	{
		printf(" * ");
		printf(known_commands[c]);
		printf("(<LedNumber>)\r\n");
	}
	printf("except for --help all command take a number from 0 to ");
	toChar(N_LAMPS,nrbfr);
	printf(nrbfr);
	printf(" as an argument,\r\n the number is the led for which the command applies\r\n");
	printf("example: RED(13) switches led 13 to red while leaving all others\r\n");
}

void toChar(uint8_t nr, char * out)
{
	uint8_t pos=100;
	uint8_t cntr=0,charpos=0;
	uint8_t firstDigit = 0;
	if (nr==0)
	{
		out[charpos++]=0x30;
	}
	else
	{
		while (pos > 0)
		{
			cntr=0;
			while (nr >= pos)
			{
				nr -= pos;
				cntr++;
			}
			if (cntr > 0 || firstDigit > 0)
			{
				out[charpos++] = cntr + 0x30;
				firstDigit = 1;
			}
			pos /= 10;
		}
	}
	out[charpos]=0;
}

uint8_t startsWith(const char* ptrn,const char* target)
{
	uint8_t cnt=0,isEqual=1;
	while (ptrn[cnt]>0 && target[cnt]>0 && isEqual > 0)
	{
		if (ptrn[cnt] != target[cnt])
		{
			isEqual=0;
		}
		cnt++;
	}
	return isEqual;
}

uint8_t toInt(char * chr)
{
	uint8_t res=0;
	uint8_t cnt=0;
	while (chr[cnt]!=0)
	{
		res *= 10;
		res += (chr[cnt] - 0x30);
		cnt++;
	}
	return res;
}

void getBracketContent(const char* input,char * out)
{
	uint8_t cnt=0,bcnt=0;
	uint8_t idxStart=255,idxEnd=0;
	char cur = *(input+cnt);
	while(cur != 0)
	{
		if (idxStart==255)
		{
			if (cur == '(')
			{
				idxStart = cnt;
			}
		}
		else if (cur != ')')
		{
			out[bcnt]=cur;
			bcnt++;
		}
		if (idxEnd==0)
		{
			if (cur == ')')
			{
				idxEnd = cnt;
			}
		}

		cnt++;
		cur = *(input+cnt);
	}
	out[bcnt]=0;
	if (bcnt==0 || idxEnd==0)
	{
		out = 0;
	}
}



