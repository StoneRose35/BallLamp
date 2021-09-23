/*
 * colorInterpolator.c
 *
 *  Created on: 21.09.2021
 *      Author: philipp
 */

#include <stdlib.h>
#ifdef STM32
#include "types.h"
#include "system.h"
#endif
#include "colorInterpolator.h"



void initTask(Task t,uint8_t nsteps)
{
	t->lamp_nr=0;
	t->Nsteps=nsteps;
	t->stepCnt=0;
	t->stepProgressionCnt=0;
	t->state=0;
	if (nsteps > 0)
	{
		void* csArray = malloc(nsteps*sizeof(ColorStepType));
		t->steps = (ColorStepType*)csArray;
	}
	else
	{
		t->steps = 0;
	}
	for(uint8_t c=0;c<nsteps;c++)
	{
		t->steps[c].r=0;
		t->steps[c].g=0;
		t->steps[c].b=0;
		t->steps[c].deltar=0;
		t->steps[c].deltag=0;
		t->steps[c].deltab=0;
		t->steps[c].frames=1;
		t->steps[c].interpolation=0;
	}
}

void setColor(Task t,uint8_t r,uint8_t g, uint8_t b,uint8_t idx)
{
	t->steps[idx].r = r;
	t->steps[idx].g = g;
	t->steps[idx].b = b;

	if (idx > 0)
	{
		t->steps[idx-1].deltar = ((t->steps[idx].r - t->steps[idx-1].r) << 8)/t->steps[idx-1].frames;
		t->steps[idx-1].deltag = ((t->steps[idx].g - t->steps[idx-1].g) << 8)/t->steps[idx-1].frames;
		t->steps[idx-1].deltab = ((t->steps[idx].b - t->steps[idx-1].b) << 8)/t->steps[idx-1].frames;
	}

	if (idx < t->Nsteps-1)
	{
		t->steps[idx].deltar = ((t->steps[idx+1].r - t->steps[idx].r) << 8)/t->steps[idx].frames;
		t->steps[idx].deltag = ((t->steps[idx+1].g - t->steps[idx].g) << 8)/t->steps[idx].frames;
		t->steps[idx].deltab = ((t->steps[idx+1].b - t->steps[idx].b) << 8)/t->steps[idx].frames;
	}
	else if (idx == t->Nsteps-1 && (t->state & (1 << 2))== (1 << 2))
	{
		t->steps[idx].deltar = ((t->steps[0].r - t->steps[idx].r) << 8)/t->steps[idx].frames;
		t->steps[idx].deltag = ((t->steps[0].g - t->steps[idx].g) << 8)/t->steps[idx].frames;
		t->steps[idx].deltab = ((t->steps[0].b - t->steps[idx].b) << 8)/t->steps[idx].frames;
	}
}

void setFrames(Task t,uint32_t nframes,uint8_t idx)
{
	t->steps[idx].frames = nframes;
	if (idx < t->Nsteps-1)
	{
		t->steps[idx].deltar = ((t->steps[idx+1].r - t->steps[idx].r) << 8)/t->steps[idx].frames;
		t->steps[idx].deltag = ((t->steps[idx+1].g - t->steps[idx].g) << 8)/t->steps[idx].frames;
		t->steps[idx].deltab = ((t->steps[idx+1].b - t->steps[idx].b) << 8)/t->steps[idx].frames;
	}
	else if (idx == t->Nsteps-1 && (t->state & (1 << 2))== (1 << 2))
	{
		t->steps[idx].deltar = ((t->steps[0].r - t->steps[idx].r) << 8)/t->steps[idx].frames;
		t->steps[idx].deltag = ((t->steps[0].g - t->steps[idx].g) << 8)/t->steps[idx].frames;
		t->steps[idx].deltab = ((t->steps[0].b - t->steps[idx].b) << 8)/t->steps[idx].frames;
	}

}

void setReady(Task t)
{
	t->stepCnt=0;
	t->stepProgressionCnt=0;
	t->state=2;
}

void updateTask(Task t,RGBStream * lampdata)
{
	if ((t->state & 0x3) ==2 && t->steps != 0) // running
	{
		if (t->stepProgressionCnt == t->steps[t->stepCnt].frames)
		{
			t->stepCnt++;
			t->stepProgressionCnt = 0;
			if(t->stepCnt==t->Nsteps)
			{
				if((t->state & (1 << 2)) == (1 << 2))
				{
					t->stepCnt = 0;
				}
				else
				{
					t->stepCnt--;
					t->state &= ~(3 << 0);
				}
			}
			t->r_cur = t->steps[t->stepCnt].r << 8;
			t->g_cur = t->steps[t->stepCnt].g << 8;
			t->b_cur = t->steps[t->stepCnt].b << 8;
		}
		else
		{

			if (t->steps[t->stepCnt].interpolation == 1)
			{
				t->r_cur += t->steps[t->stepCnt].deltar;
				t->g_cur += t->steps[t->stepCnt].deltag;
				t->b_cur += t->steps[t->stepCnt].deltab;
			}
		}
		lampdata[t->lamp_nr].rgb.r = t->r_cur >> 8;
		lampdata[t->lamp_nr].rgb.g = t->g_cur >> 8;
		lampdata[t->lamp_nr].rgb.b = t->b_cur >> 8;
		t->stepProgressionCnt++;
	}
	else if ((t->state & 0x3) == 1) // starting
	{
		t->state += 1;
		t->r_cur = t->steps[t->stepCnt].r << 8;
		t->g_cur = t->steps[t->stepCnt].g << 8;
		t->b_cur = t->steps[t->stepCnt].b << 8;

		lampdata[t->lamp_nr].rgb.r = t->r_cur >> 8;
		lampdata[t->lamp_nr].rgb.g = t->g_cur >> 8;
		lampdata[t->lamp_nr].rgb.b = t->b_cur >> 8;
		//t->stepProgressionCnt++;

	}

}
