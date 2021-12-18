#include "systemChoice.h"

#ifdef HARDWARE
#ifndef SIMPLE_TIMERTEST
#ifndef SIMPLE_NEOPIXEL



#include <neopixelDriver.h>
#include "system.h"
#include "core.h"
#include "systemClock.h"
#include "uart.h"
#include "consoleHandler.h"
#include "apiHandler.h"
#include "bufferedInputHandler.h"
#include "colorInterpolator.h"
#include "interpolators.h"
#include "stringFunctions.h"
#include "taskManager.h"


RGBStream lampsdata[N_LAMPS];
RGBStream * lamps = lampsdata;
uint32_t clr_cnt = 0;
uint32_t bit_cnt = 0;

uint8_t rawdata[N_LAMPS*24+1];
uint8_t* rawdata_ptr = rawdata;

TaskType interpolatorsArray[N_LAMPS];
TasksType interpolators;


volatile uint32_t task;
volatile uint8_t context;

extern CommBufferType usbCommBuffer;
extern CommBufferType btCommBuffer;

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
	ConsoleType btConsole;
	ApiType usbApi;
	ApiType btApi;
	BufferedInputType usbInput;
	BufferedInputType btInput;


	#ifdef STM32
	enableFpu();
	#endif
    setupClock();


    initConsole(&usbConsole);
    initConsole(&btConsole);
    initApi(&usbApi);
    initApi(&btApi);

    usbInput.api = &usbApi;
    usbInput.console = &usbConsole;
    usbInput.commBuffer=&usbCommBuffer;
    usbInput.interfaceType=BINPUT_TYPE_CONSOLE;

    btInput.api = &btApi;
    btInput.console = &btConsole;
    btInput.commBuffer=&btCommBuffer;
    btInput.interfaceType=BINPUT_TYPE_CONSOLE;

	initGpio();
	initBTUart();
	initUart();


	context |= (1 << CONTEXT_USB) | (1 << CONTEXT_BT);
	printf("initializing color interpolators\r\n");

	interpolators.taskArray=(TaskType*)interpolatorsArray;
	interpolators.taskArrayLength=N_LAMPS;
	initInterpolators(&interpolators);

	startInterpolators(&interpolators);

	setSendState(SEND_STATE_RTS);

	initTimer();


	printf("BallLamp v0.1 running\r\n");

    /* Loop forever */
	for(;;)
	{
		
		if (getSendState()==SEND_STATE_RTS)//(READY_TO_SEND)
		{
			sendToLed(); // non-blocking, returns long before the neopixel clock pulses have been sent
			tasksDone = 0;
		}
		

		if ((task & (1 << TASK_USB_CONSOLE))==(1 << TASK_USB_CONSOLE))
		{
			context = (1 << CONTEXT_USB);
			processInputBuffer(&usbInput);

			task &= ~(1 << TASK_USB_CONSOLE);
		}

		if ((task & (1 << TASK_BT_CONSOLE))==(1 << TASK_BT_CONSOLE))
		{
			context = (1 << CONTEXT_BT);
			processInputBuffer(&btInput);

			task &= ~(1 << TASK_BT_CONSOLE);
		}
		//
		// Time slot for handling tasks
		// 
		if (tasksDone == 0)
		{
			for(uint8_t c=0;c<interpolators.taskArrayLength;c++)
			{
				if ((interpolators.taskArray[c].state & 0x3) != TASK_STATE_STOPPED)
				{
					updateTask(&interpolators.taskArray[c],lamps);
				}
			}
			tasksDone=1;
		}

		// if the tasks are finished after the fps time has elapsed the next frame doesn't show
		// the correct data due to a buffer underrun
		
		if(getSendState()==SEND_STATE_BUFFER_UNDERRUN)
		{
			// potential error handling
			context |= (1 << CONTEXT_USB) | (1 << CONTEXT_BT);
			printf("BufferUnderrun!!\r\n");
		}

		if (getSendState()==SEND_STATE_SENT || getSendState()==SEND_STATE_BUFFER_UNDERRUN)// is in wait state after after the data transfer
		{
			decompressRgbArray(lamps,N_LAMPS);
		}
		
		sendCharAsyncUsb();
		sendCharAsyncBt();

	}
}
#endif
#endif
#endif
