#include <stdio.h>
#include "consoleHandler.h"
#include "taskManager.h"
#include "system.h"
#include "stringFunctions.h"
#include "demoColorInterpolator.h"
#include "colorInterpolator.h"
#include "interpolators.h"


TaskType interpolatorsArray[N_LAMPS];
TasksType interpolators;

void consoleHandlerHistoryCheck()
{
	ConsoleType c1;

	RGBStream lamps[N_LAMPS];

	onCharacterReception(&c1,(uint8_t)'B',lamps);
	onCharacterReception(&c1,(uint8_t)'L',lamps);
	onCharacterReception(&c1,(uint8_t)'U',lamps);
	onCharacterReception(&c1,(uint8_t)'E',lamps);
	onCharacterReception(&c1,(uint8_t)'(',lamps);
	onCharacterReception(&c1,(uint8_t)'0',lamps);
	onCharacterReception(&c1,(uint8_t)')',lamps);
	onCharacterReception(&c1,(uint8_t)'\r',lamps);

	// simulate up arrow
	onCharacterReception(&c1,(uint8_t)'\e',lamps);
	onCharacterReception(&c1,(uint8_t)'[',lamps);
	onCharacterReception(&c1,(uint8_t)'A',lamps);

	// delete three characters
	onCharacterReception(&c1,127,lamps);
	onCharacterReception(&c1,127,lamps);
	onCharacterReception(&c1,127,lamps);

	// write a "1", text should now be BLUE1
	onCharacterReception(&c1,(uint8_t)'1',lamps);


	onCharacterReception(&c1,(uint8_t)'\r',lamps);

	handleHelp(0,0);
	printf("done");
}

void testExpandRange()
{
	uint8_t * resultdata;
	uint8_t len;
	char rangedef[16]="3-7";

	len=expandRange(rangedef,&resultdata);
	if (resultdata[0] - 3 != 0)
	{
		printf("test failed");
	}
	if (resultdata[4] - 7 != 0)
	{
		printf("test failed");
	}
	if(len!=5)
	{
		printf("test failed, length should be 5");
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
		printf("test failed, length should be 1");
	}

	char descr2[] = "4,2,2,7";
	len = expandLampDescription(descr2,&lampnrarray);
	if(len != 3)
	{
		printf("test failed, length should be 3");
	}
	if (lampnrarray[0]!= 2)
	{
		printf("test failed, first element should be 2");
	}

	char descr3[] = "4,2,7, 3 - 6";
	len = expandLampDescription(descr3,&lampnrarray);
	if(len != 6)
	{
		printf("test failed, length should be 6");
	}
}


void testRgbCommand()
{
	RGBStream lamps[N_LAMPS];
	char command[32] = "RGB(23,34,45,1)";
	handleCommand(command,lamps);
	if (lamps[1].rgb.r != 23 || lamps[1].rgb.g != 34 || lamps[1].rgb.b != 45)
	{
		printf("test failed: color set for lamp 1 is incorrect");
	}
}

int main(int argc,char** argv)
{
	interpolators.taskArray=(TaskType*)interpolatorsArray;
	interpolators.taskArrayLength=N_LAMPS;
	initInterpolators(&interpolators);

	//consoleHandlerHistoryCheck();
	testExpandRange();
	testExpandDescription();
	testRgbCommand();
	demoColorInterpolator();
}




