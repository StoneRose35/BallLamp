/*
 * interpolators.c
 *
 *  Created on: 21.10.2021
 *      Author: philipp
 */


#include "interpolators.h"
#include "colorInterpolator.h"
#include <stdlib.h>

void initInterpolators(Tasks tasks)
{
	for(uint8_t c=0;c<tasks->taskArrayLength;c++)
	{
		tasks->taskArray[c].Nsteps=0;
		tasks->taskArray[c].b_cur=0;
		tasks->taskArray[c].g_cur=0;
		tasks->taskArray[c].r_cur=0;
		tasks->taskArray[c].lamp_nr=255;
		tasks->taskArray[c].state=0;
		tasks->taskArray[c].stepCnt=0;
		tasks->taskArray[c].stepProgressionCnt=0;
		tasks->taskArray[c].steps=0;
	}
}

uint8_t setLampInterpolator(Tasks tasks,uint8_t lampNr,uint8_t nsteps,uint8_t repeating)
{
	uint8_t taskCreated=0;
	uint8_t firstFreeIndex=255;
	for(uint8_t c=0;c<tasks->taskArrayLength;c++)
	{
		if (tasks->taskArray[c].lamp_nr == lampNr)
		{
			if ((tasks->taskArray[c].state & 0x3) != STATE_STOPPED) // there is an interpolator running for the lamp return an error code
			{
				return 1;
			}
			else
			{
				destroyTask(&tasks->taskArray[c]);
			}
			initTask(&tasks->taskArray[c],nsteps,lampNr);
			tasks->taskArray[c].state &= ~0x4;
			tasks->taskArray[c].state |= repeating << 2;
			taskCreated=1;
		}
		else if (tasks->taskArray[c].steps == 0 && firstFreeIndex == 0xFF)
		{
			firstFreeIndex=c;
		}
	}
	if (taskCreated == 0 )
	{
		if (firstFreeIndex != 0xFF)
		{
			if (tasks->taskArray[firstFreeIndex].steps != 0)
			{
				destroyTask(&tasks->taskArray[firstFreeIndex]);
			}
			initTask(&tasks->taskArray[firstFreeIndex],nsteps,lampNr);
			tasks->taskArray[firstFreeIndex].state &= ~0x4;
			tasks->taskArray[firstFreeIndex].state |= repeating << 2;
			tasks->taskArray[firstFreeIndex].state |= STATE_STARTING;
		}
		else
		{
			return 1;
		}
	}
	return 0;
}

uint8_t getLampIndex(Tasks tasks,uint8_t lampnr)
{
	for(uint8_t c=0;c<tasks->taskArrayLength;c++)
	{
		if(tasks->taskArray[c].lamp_nr == lampnr)
		{
			return c;
		}
	}
	return 0xFF;
}

uint8_t setColorFramesInterpolation(Tasks tasks, uint8_t r,uint8_t g,uint8_t b,int16_t frames,uint8_t interpolation, uint8_t lamp_nr,uint8_t step)
{
	uint8_t idx;
	idx=getLampIndex(tasks,lamp_nr);
	if(idx != 0xFF)
	{
		if (step >= tasks->taskArray[idx].Nsteps)
		{
			return ERR_INVALID_STEP;
		}
		tasks->taskArray[idx].steps[step].interpolation = interpolation;
		setColor(&tasks->taskArray[idx],r,g,b,step);
		setFrames(&tasks->taskArray[idx],frames,step);

	}
	return 0;
}

uint8_t startInterpolators(Tasks tasks)
{
	for (uint8_t c=0;c<tasks->taskArrayLength;c++)
	{
		if (tasks->taskArray[c].steps != 0)
		{
			start(&tasks->taskArray[c]);
		}
	}
	return 0;
}

uint8_t stopInterpolators(Tasks tasks)
{
	for (uint8_t c=0;c<tasks->taskArrayLength;c++)
	{
		stop(&tasks->taskArray[c]);
	}
	return 0;
}

uint16_t* toStream(Tasks t)
{
	uint8_t* resultStream;
	uint32_t tasksSize = 0;
	uint32_t offset=0,offsetStream = 0;
	tasksSize += sizeof(TasksType) - 4; // excluding the pointer to the array
	for(uint8_t c=0;c<t->taskArrayLength;c++)
	{
		tasksSize += sizeof(TaskType) - 4; // again removing the pointer
		tasksSize += t->taskArray[c].Nsteps*sizeof(ColorStepType);
	}
	if ((tasksSize & 0x1) == 0x1) // add a zero byte to get an integer half-word size
	{
		tasksSize++;
	}
	resultStream=(uint8_t*)malloc(tasksSize);

	// store tasks header information (the nubmer of TaskType's and the name)
	offset = 4;
	for(uint8_t c=0;c<33;c++)
	{
		*(resultStream + offsetStream++)=*((uint8_t*)t+offset++);
	}

	// store TaskType's
	for(uint8_t c=0;c<t->taskArrayLength;c++)
	{
		// loop through TaskType
		offset = 4;
		for (uint8_t c2=0;c2<sizeof(TasksType) - 4;c2++)
		{
			*(resultStream + offsetStream++) = *((uint8_t*)(t->taskArray + c)+offset++);
		}
		// store ColorStepTypes
		for (uint8_t c3=0;c3<t->taskArray[c].Nsteps;c3++)
		{
			for(uint8_t c4=0;c4<sizeof(ColorStepType);c4++)
			{
				*(resultStream + offsetStream++) = *((uint8_t*)(t->taskArray[c].steps + c4) + c4);
			}
		}
	}
	return (uint16_t*)resultStream;
}

void fromStream(uint16_t* stream,Tasks t)
{
	uint32_t offset = 0;
	uint8_t* byteStream=(uint8_t*)stream;
	t->taskArrayLength = *(byteStream+offset++);
	for(uint8_t c=0;c<32;c++)
	{
		*(t->name + c) = *(byteStream+offset++);
	}
	TaskType* tarr=(TaskType*)malloc(t->taskArrayLength*sizeof(TaskType));
	t->taskArray=tarr;

}

