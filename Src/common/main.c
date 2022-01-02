/**
 * \mainpage Ico Lamp Overview
 * \section archoverview Architectural Overview
 * The system contains two following basic components
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
 * \section dataStructures Data Structures
 * Regarding the neopixel control the following data structures exists as singletons in the system.
 * * *rawdata* as a DMA-streamable array consumed by the hardware and converted from RGBStream by the neopixelDriver.c. This array
 * should never be manipulated by user code.
 * * *RGBStream* as a structures static or one-frame color information for all neopixels. Can be modified by user code. Note that editing these
 * values while the animation is running can have an unpredictable effect on the neopixel colors.
 * * *Tasks* as an animation definition. This structures should only be edited while the animation is not running.
 * \section commandref User Command Reference
 * \subsection colorCommands Color Command
 * `<COLOR>(neopixels)`
 * 
 * The following command exists which set a set of neopixel to a predefined color
 * * BACKGROUND
 * * FOREGROUND
 * * RED
 * * GREEN
 * * DARKBLUE
 * * LIGHTBLUE
 * * MAGENTA
 * * YELLOW
 * * ORANGE
 * * PURPLE
 * * YELLOWGREEN
 * * MEDIUMBLUE
 * * DARKYELLOW
 * * AQUA
 * * DARKPURPLE
 * * GRAY
 * The comand takes the neopixel set as an argument. Range declarations and direct indexes can ben combines freely.
 * Example AQUA(0-4,10) set the neopixel 0,1,2,3,4,10 to the color aqua.
 * \subsection rgbCommand RGB Command
 * `%RGB(Red,Green,Blue,neopixels)`
 * 
 * This command allow to set neopixels to a specific %RGB color value. The individual color values are in the range 0-255. Example %RGB(110,0,110,1,3,5-7) set the neopixels 1,3,5,6,7
 * to a dim violet.
 * \subsection startcommand Start Command
 * `START`
 * 
 * Starts the neopixel animation, if possible. 
 * \subsection stopcommand Stop Command
 * `STOP`
 * 
 * Stops the neopixel animation, if possible.
 * \subsection interpCommand Interpolator Setup Command
 * `INTERP(lamp_nr,nSteps,repeating)`
 * 
 * defines an interpolator/color sequence for neopixel lampnr, 
 * 255 means every lamp with nSteps steps, loops if repeating is 1, executes as one-shot if 0 . Example: INTERP(2,4,0)
 * defines an color sequence with 4 steps for neoppixel 2 which runs as a one-shot.
 * \subsection istepCommand Intepolator Step setup Command
 * `ISTEP(r,g,b,frames,interpolation,lampnr,step)`
 * 
 * r,g and b defines the color value from 0 to 255, frames define the number of frames it should take to display to 
 * color (if interplation is set constant) or to translate to the next color (if interpolation is linear). interpolation is 0 for
 * constant and 1 for linear. lampnr is the neopixel index starting from 0. step denotes the index in the steps array for which the color 
 * and duration should be set.
 * \subsection desciCommand Color Sequence / Interpolator description
 * `DESCI`
 * 
 * Print a description of a color sequences / interpolators including their progression. 
 * \subsection destroyCommand Destroy Color Sequence / Interpolator
 * `DESTROY(lampnr)`
 * 
  * lampnr: the neopixel whose interpolator should be destroyed, 255 for every neopixel/all interpolators.
 * \subsection npEngingeCommand Switch Neopixel Engine On/Off
 * `NPENGINGE(flag)`
 * 
 * switches the neopixel engine on or off, if off no interrupts are generated, the neopixel itself keeps the color last set.
 * off if flag is 0, on if flag is 1
 * \subsection saveCommand Save Command
 * `SAVE`
 * 
 * Save the current animation persistently. Can be loaded again using LOAD. Untested and probably not working in the Raspberry Pi Pico implementation.
 * \subsection loadCommand Load Command
 * `LOAD`
 * 
 * Loads an animation from persistent storage. Behaves unpredictable if not animation has been saved previously. Untested and probably not working in the Raspberry Pi Pico implementation.
 * \subsection apiCommand API Mode Command
 * `API`
 * 
 * Switches the current interface to API mode. This is typically used by a system interface such as the Android App also available in the project.
 * If this command is issued as a user the interface may suddenly appear unresponsive since the characters typed in are note echoed anymore.
 * In this case issue the Command "CONSOLE" to switch back to CLI/Console mode.
 * \subsection consoleCommand Console/CLI Mode Command
 * `CONSOLE`
 * 
 * Switches the current interface back to CLI mode. This command is useful in debugging cases or when the interface mode has been set to API erroneously.
 * \subsection setupbtCommand Setup Bluetooth Command
 * `SETUPBT`
 * 
 * Configures the HC-06 Bluetooth interface (Name, PIN and Baudrate) using AT-Commands. Not implemented in the Raspberry Pi Pico Version, Deprecated, will be removed in the future.
 * \subsection sysinfoCommand Sysinfo Command
 * `SYSINFO`
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
 * * initialize the neopixel driver itself with initNeopixels
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
#include "systick.h"
#include "uart.h"
#include "spi_sdcard_display.h"
#include "consoleHandler.h"
#include "apiHandler.h"
#include "bufferedInputHandler.h"
#include "colorInterpolator.h"
#include "interpolators.h"
#include "stringFunctions.h"
#include "taskManager.h"
#include "neopixelCommands.h"


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
	initSystickTimer();

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
	initBTUart(BAUD_RATE);
	initUart(BAUD_RATE);
	initSpi();


	context |= (1 << CONTEXT_USB) | (1 << CONTEXT_BT);
	printf("initializing color interpolators\r\n");

	interpolators.taskArray=(TaskType*)interpolatorsArray;
	interpolators.taskArrayLength=N_LAMPS;
	initInterpolators(&interpolators);

	startInterpolators(&interpolators);

	setSendState(SEND_STATE_RTS);

	initNeopixels();

	/**
	 * set initial color upon start
	 */
	/*
	for (uint8_t c=0;c<N_LAMPS;c++)
	{
		lamps[c].rgb.r = 245;
		lamps[c].rgb.g = 140;
		lamps[c].rgb.b = 40;
	}
	*/


	printf("Microsys v1.0 running\r\n");

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
