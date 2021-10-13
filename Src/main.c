#ifdef STM32
/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */


#include <neopixelDriver.h>
#include "system.h"
#include "systemClock.h"
#include "uart.h"
#include "consoleHandler.h"
#include "colorInterpolator.h"
#include "stringFunctions.h"
#include "taskManager.h"


RGBStream lampsdata[N_LAMPS];
RGBStream * lamps = lampsdata;
uint32_t clr_cnt = 0;
uint32_t bit_cnt = 0;

uint8_t rawdata[N_LAMPS*24+1];
uint8_t* rawdata_ptr = rawdata;

TaskType interpolators[N_LAMPS];

volatile uint32_t task;

extern volatile uint8_t inputBuffer[8];
extern volatile uint8_t inputBufferCnt;

/*
 * updates the color along a hue shift with the phase going from 0 to 1535
 * */
void colorUpdate(RGB * color,uint32_t phase)
{
	if (phase < 0x100)
	{
		color->r = 0xFF;
		color->g = phase & 0xFF;
		color->b = 0x00;
	}
	else if (phase < 0x200)
	{
		color->r = 0x1FF - phase;
		color->g = 0xFF;
		color->b = 0x00;
	}
	else if (phase < 0x300)
	{
		color->r = 0x00;
		color->g = 0xFF;
		color->b = phase - 0x200;
	}
	else if (phase < 0x400)
	{
		color->r = 0x00;
		color->g = 0x3FF - phase;
		color->b = 0xFF;
	}
	else if (phase < 0x500)
	{
		color->r = phase - 0x400;
		color->g = 0x00;
		color->b = 0xFF;
	}
	else
	{
		color->r = 0xFF;
		color->g = 0x00;
		color->b = 0x5FF - phase;
	}
}


int main(void)
{
	uint8_t tasksDone = 1;
	ConsoleType usbConsole;
    setupClock();

	initTimer();
	initUart();

	printf("initializing color interpolators\r\n");
	for (uint8_t c=0;c<N_LAMPS;c++)
	{
		initTask(interpolators+c,0);
	}



	// test hack
	/*
	initTask(interpolators+10,4);
	(interpolators+10)->lamp_nr=10;
	setColor(interpolators+10,0,0,0,0);
	setColor(interpolators+10,0,255,0,1);
	setColor(interpolators+10,0,45,130,2);
	setColor(interpolators+10,200,200,200,3);
	(interpolators+10)->state |= (1 << 2);
	setFrames((interpolators+10),13,0);
	setFrames((interpolators+10),5,1);
	setFrames((interpolators+10),12,2);
	setFrames((interpolators+10),27,3);
	(interpolators+10)->state |= (1 << 0);
	(interpolators+10)->steps[0].interpolation=1;
	(interpolators+10)->steps[1].interpolation=1;
	(interpolators+10)->steps[2].interpolation=1;
	(interpolators+10)->steps[3].interpolation=1;

	initTask(interpolators,2);
	interpolators->lamp_nr=1;
	setColor(interpolators,0,0,0,0);
	setColor(interpolators,240,0,0,1);
	setFrames(interpolators,5,0);
	setFrames(interpolators,5,1);
	interpolators->state |= (1 << 2);
	interpolators->state |= (1 << 0);
    */

	//decompressRgbArray(frame,N_LAMPS);
	setSendState(SEND_STATE_RTS);


	printf("BallLamp v0.1 running\r\n");

    /* Loop forever */
	for(;;)
	{

		if (getSendState()==SEND_STATE_RTS)//(READY_TO_SEND)
		{
			sendToLed(); // non-blocking, returns long before the neopixel clock pulses have been sent
			tasksDone = 0;
		}

		if ((task & (1 << TASK_CONSOLE))==(1 << TASK_CONSOLE))
		{
			while(inputBufferCnt > 0)
			{
				char* consoleBfr;
				consoleBfr = onCharacterReception(&usbConsole,inputBuffer[inputBufferCnt-1],lamps);
				printf(consoleBfr);
				inputBufferCnt--;
			}
			task &= ~(1 << TASK_CONSOLE);
		}
		/*
		 * Time slot for handling tasks
		 */
		if (tasksDone == 0)
		{
			for(uint8_t c=0;c<N_LAMPS;c++)
			{
				if (((interpolators+c)->state & 0x3) != TASK_STATE_STOPPED)
				{
					updateTask(interpolators+c,lamps);
				}
			}
			tasksDone=1;
		}

		// if the tasks are finished after the fps time has elapsed the next frame doesn't show
		// the correct data due to a buffer underrun
		if(getSendState()==SEND_STATE_BUFFER_UNDERRUN)
		{
			// potential error handling
			printf("BufferUnderrun!!\r\n");
		}

		if (getSendState()==SEND_STATE_SENT || getSendState()==SEND_STATE_BUFFER_UNDERRUN)// is in wait state after after the data transfer
		{
			decompressRgbArray(lamps,N_LAMPS);
		}

		sendCharAsync();

	}
}
#endif
