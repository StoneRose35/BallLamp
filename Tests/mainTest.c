#include <stdio.h>
#include "consoleHandler.h"
#include "taskManager.h"
#include "taskManagerUtils.h"
#include "system.h"
#include "stringFunctions.h"
#include "demoColorInterpolator.h"
#include "colorInterpolator.h"
#include "interpolators.h"


TaskType interpolatorsArray[N_LAMPS];
TasksType interpolators;

RGBStream lampsarray[N_LAMPS];
RGBStream * lamps=lampsarray;

void consoleHandlerHistoryCheck()
{
	ConsoleType c1;

	initConsole(&c1);

	onCharacterReception(&c1,(uint8_t)'A',lamps);
	onCharacterReception(&c1,(uint8_t)'Q',lamps);
	onCharacterReception(&c1,(uint8_t)'U',lamps);
	onCharacterReception(&c1,(uint8_t)'A',lamps);
	onCharacterReception(&c1,(uint8_t)'(',lamps);
	onCharacterReception(&c1,(uint8_t)'0',lamps);
	onCharacterReception(&c1,(uint8_t)')',lamps);
	onCharacterReception(&c1,(uint8_t)'\r',lamps);

	// simulate up arrow
	onCharacterReception(&c1,(uint8_t)'\e',lamps);
	onCharacterReception(&c1,(uint8_t)'[',lamps);
	onCharacterReception(&c1,(uint8_t)'A',lamps);

	// delete two characters
	onCharacterReception(&c1,127,lamps);
	onCharacterReception(&c1,127,lamps);

	// write a "1)", text should now be AQUA(1)
	onCharacterReception(&c1,(uint8_t)'1',lamps);
	onCharacterReception(&c1,(uint8_t)')',lamps);


	onCharacterReception(&c1,(uint8_t)'\r',lamps);

	if (!(lamps->rgb.r == 0 && lamps->rgb.g == 210 && lamps->rgb.b == 140))
	{
		printf("test failed, color of first lamp is wrong\n");
	}

	if (!((lamps+1)->rgb.r == 0 && (lamps+1)->rgb.g == 210 && (lamps+1)->rgb.b == 140))
	{
		printf("test failed, color of second lamp is wrong\n");
	}


}

void testExpandRange()
{
	uint8_t * resultdata;
	uint8_t len;
	char rangedef[16]="3-7";

	len=expandRange(rangedef,&resultdata);
	if (resultdata[0] - 3 != 0)
	{
		printf("test failed\n");
	}
	if (resultdata[4] - 7 != 0)
	{
		printf("test failed\n");
	}
	if(len!=5)
	{
		printf("test failed, length should be 5\n");
	}
}

void testExpandDescription()
{
	char descr[16]="4";
	uint8_t len;
	uint8_t * lampnrarray;
	len = expandLampDescription(descr,&lampnrarray);
	if(len != 1)
	{
		printf("test failed, length should be 1\n");
	}

	char descr2[] = "4,2,2,7";
	len = expandLampDescription(descr2,&lampnrarray);
	if(len != 3)
	{
		printf("test failed, length should be 3\n");
	}
	if (lampnrarray[0]!= 2)
	{
		printf("test failed, first element should be 2\n");
	}

	char descr3[] = "4,2,7, 3 - 6";
	len = expandLampDescription(descr3,&lampnrarray);
	if(len != 6)
	{
		printf("test failed, length should be 6\n");
	}
}


void testRgbCommand()
{
	char command[32] = "RGB(23,34,45,1)";
	handleCommand(command);
	if (lamps[1].rgb.r != 23 || lamps[1].rgb.g != 34 || lamps[1].rgb.b != 45)
	{
		printf("test failed: color set for lamp 1 is incorrect\n");
	}
}

void testConvertInts()
{
	uint8_t v1;
	int16_t v2;
	v1 = toUInt8("56");
	if (v1 != 56)
	{
		printf("test failed: should return 56\n");
	}

	v2 = toInt16("-134");
	if (v2 != -134)
	{
		printf("test failed: should return -134\n");
	}

	v2 = toInt16("13443");
	if (v2 != 13443)
	{
		printf("test failed: should return 13443\n");
	}

}

void testPercentToChar()
{
	char nrbfr[8];
	float a=0.784528;

	toPercentChar(a,nrbfr);
	if (!(nrbfr[0] == '7' &&
			nrbfr[1] == '8' &&
			nrbfr[2] == '.' &&
			nrbfr[3] == '4' &&
			nrbfr[4] == '5' &&
			nrbfr[5] == '2' ))
	{
		printf("test failed, returned string is not 78.452\n");
	}
}

void testFillWithLeadingZeros()
{
	char nrbfr[8];
	nrbfr[0]=0x30;
	nrbfr[1]=0;

	fillWithLeadingZeros(4,nrbfr);
	if ((uint8_t)nrbfr[0] != 0x30)
	{
		printf("testFillWithLeadingZeros failed, pos 1 is not 0");
	}
	if ((uint8_t)nrbfr[1] != 0x30)
	{
		printf("testFillWithLeadingZeros failed, pos 2 is not 0");
	}
	if ((uint8_t)nrbfr[2] != 0x30)
	{
		printf("testFillWithLeadingZeros failed, pos 3 is not 0");
	}
	if ((uint8_t)nrbfr[3] != 0x30)
	{
		printf("testFillWithLeadingZeros failed, pos 4 is not 0");
	}
	if ((uint8_t)nrbfr[4] != 0x0)
	{
		printf("testFillWithLeadingZeros failed, string is not zero-terminated");
	}
}


int main(int argc,char** argv)
{
	interpolators.taskArray=(TaskType*)interpolatorsArray;
	interpolators.taskArrayLength=N_LAMPS;
	initInterpolators(&interpolators);

	consoleHandlerHistoryCheck();
	testExpandRange();
	testExpandDescription();
	testRgbCommand();
	demoColorInterpolator();
	testConvertInts();
	testPercentToChar();
	testFillWithLeadingZeros();
	return 0;
}




