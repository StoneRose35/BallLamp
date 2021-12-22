/**
 * \mainpage Ico Lamp Overview
 * \section archoverview Architectural Overview
 * The system contains two following basinc components
 * \subsection comminterfaces Communication Interfaces
 * Two Uart Communication interfaces accessible through usb and bluetooth exposing a command line interface (CLI) or
 * and application programming interface (API). The interface type can be changed at runtime. The properties of the communication
 * interfaces are defined in bufferedInputStructs.h. The baudrate is defined in uart.h.
 * \subsection neopixeldriver Neopixel driver
 * A hardware-based neopixel driver allowing the transmission of a neopixel color array without interruption. This is typically
 * achieved with a DMA-fed timer or state machine.
 * \subsection fakebash Fake-Bash
 * The CLI features a defineable command history and basic cursor capabilities known from bash excluding autocompletion. 
 * The command line prefix is defined in consoleHandler.h
 * \image html ./timing_diagram_large.png
 * also see Doc/timing_diagram.svg
 * \section commandref User Command Reference
 * 
 * 
 */


/**
 * @file main.c
 * @author Philipp Fuerholz (fuerholz@gmx.ch)
 * @brief Entry Point for the Ico Lamp system
 * @version 0.1
 * @date 2021-12-22
 * ##Startup##
 * Assumes an initialized C runtime environment, this means 
 * * Stack Pointer is defined
 * * bss section i zero-initialized
 * * data section contains the values defined in the flash image
 * ##Main Function##
 * The main function is split into an initialization part run only once and a loop part running infinitly.
 * ###Initialization part###
 * In the initialization phase the following steps can be implemented
 * * clock setup: Set the CPU and bus clock according to the specific needs
 * * initialize the CPU itself, for example enable the floating point unit
 * * initialize communication interfaces: setting up the data structures is handled in a hardware-independent manner. A specific 
 *   implementation should provide initializers for the physical interface (initGpio) and the interrupt handlers for the communication interfaces
 *   (initUart and initBTUart)
 * * initialize the neopixel driver itself with initTimer \todo choose a better name for this
 * ###Main Loop###
 * The main loops processes a serie of tasks in a predefined order if they should be executed, these are
 * * send raw color data to the neopixel array
 * * handle usb CLI/API
 * * handle bluetooth CLI/API
 * * update Task array, e.g. compute the colors for the next frame
 * * convert RGBStream to streamable color data
 * * send one character over the usb Uart
 * * send one character over the bluetooth Uart
 */
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

/**
 * @brief updates the color along a hue shift with the phase going from 0 to 1535
 * @param color the color data to update
 * @param phase the phase value going from 0x0 to 0x600, the hue shifts starts/ends with red
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

/**
 * @brief the main entry point, should never exit
 * 
 * @return int by definition but should never return a value
 */
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
