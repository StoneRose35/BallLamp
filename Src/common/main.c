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
//#include "romfunc.h"
#include "systemClock.h"
#include "systick.h"
//#include "datetimeClock.h"
#include "uart.h"
#include "dma.h"
#include "pio.h"
//#include "pwm.h"
#include "adc.h"
#include "timer.h"
//#include "spi_sdcard_display.h"
#include "ssd1306_display.h"
#include "debugLed.h"
//#include "fatLib.h"
#include "consoleHandler.h"
#include "apiHandler.h"
#include "bufferedInputHandler.h"
//#include "colorInterpolator.h"
//#include "interpolators.h"
#include "stringFunctions.h"
//#include "taskManager.h"
//#include "neopixelCommands.h"
#include "charDisplay.h"
//#include "ds18b20.h"
//#include "remoteSwitch.h"
#include "rotaryEncoder.h"
#include "cliApiTask.h"
//#include "uiStack.h"
//#include "heater.h"
//#include "services/triopsBreederService.h"
//#include "services/hibernateService.h"
#include "i2s.h"
#include "audio/sineplayer.h"
#include "audio/simpleChorus.h"
#include "audio/secondOrderIirFilter.h"
#include "audio/firFilter.h"
#include "audio/waveShaper.h"
#include "audio/oversamplingWaveshaper.h"
#include "multicore.h"
#include "core1Main.h"
#include "audio/fxprogram/fxProgram.h"


volatile uint32_t task=0;
volatile uint8_t context;

extern CommBufferType usbCommBuffer;
extern CommBufferType btCommBuffer;


int16_t* audioBufferPtr;
#ifndef I2S_INPUT
uint16_t* audioBufferInputPtr;
#else
int16_t* audioBufferInputPtr;
#endif
int16_t inputSample, inputSampleOther;
uint32_t inputWord;
uint32_t rotatemaske;
uint8_t carrybitOld=0,carrybit=0;
uint32_t wordout;
uint32_t core1Handshake;
volatile int16_t avgOut=0,avgOutOld=0,avgIn=0,avgInOld=0;
uint16_t bufferCnt=0;
volatile uint8_t fxProgramIdx = 1;
volatile uint32_t ticStart,ticEnd,cpuLoad;
#define UI_UPDATE_IN_SAMPLE_BUFFERS 300
#define AVERAGING_LOWPASS_CUTOFF 10

/**
 * @brief the main entry point, should never exit
 * 
 * @return int by definition but should never return a value
 */
int main(void)
{


	/*
	 *
	 * Initialize Hardware components
	 * 
	 * */
	#ifdef STM32
	enableFpu();
	#endif
    setupClock();

	startCore1(&core1Main);
	// sync with core 1
	
	while ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB)) != (1 << SIO_FIFO_ST_VLD_LSB));
	core1Handshake=*SIO_FIFO_RD;
	while (core1Handshake != 0xcafeface)
	{
		DebugLedOn();
		core1Handshake = *SIO_FIFO_RD;
	}
	

	initUsbPll();
	initSystickTimer();
	initDMA();
	//initPwm();
	initPio();
	initGpio();
	//initSpi();
	//initDatetimeClock();
	initTimer();
	//initUart(BAUD_RATE);
	initAdc();
	//


	/*
	 *
	 * Initialise Component-specific drivers
	 * 
	 * */
	//initCliApi();
	initSsd1306Display();
	initI2S();
	enableAudioEngine(); // i2s adc and dac
	initDebugLed();
	initRotaryEncoder();


	/*
     *
     * Initialize Background Services
     *
	 */

	initRoundRobinReading(); // internal adc for reading parameters

	//printf("Microsys v1.0 running\r\n");
	ssd1306ClearDisplay();
	for (uint8_t c=0;c<N_FX_PROGRAMS;c++)
	{
		fxPrograms[c]->setup(fxPrograms[c]->data);
	}
	ssd1306WriteText(fxPrograms[fxProgramIdx]->name,0,0);

	ticEnd=0;
	ticStart=0;
	setNote(64);
    /* Loop forever */
	for(;;)
	{

		//cliApiTask(task);
		if ((task & (1 << TASK_PROCESS_AUDIO))!= 0)
		{
			ticStart = getTimeLW();


			audioBufferPtr = getEditableAudioBuffer();
			#ifndef I2S_INPUT
			audioBufferInputPtr = getReadableAudioBuffer();
			#else
			audioBufferInputPtr = getInputAudioBuffer();
			#endif

			for (uint8_t c=0;c<AUDIO_BUFFER_SIZE;c++)
			{
				// convert raw input to signed 16 bit
				#ifndef I2S_INPUT
				inputSample = (*(audioBufferInputPtr + c) << 4) - 0x7FFF;
				#else

				// the input has to be rotated right by two pixels for some strange reasons ... to be fixed later
				// foldover distortion happens when no shifting is done

				inputSample=*(audioBufferInputPtr + c*2);
				inputSampleOther=*(audioBufferInputPtr + c*2+1);
				inputWord = ((uint16_t)inputSample << 16) | (0xFFFF & ((uint16_t)inputSampleOther));

				rotatemaske = inputWord << (32-3);
				inputWord = (inputWord >> 3) | rotatemaske;
				inputSample =   0xFFFF & (inputWord >> 16);

				#endif
				//inputSample = getNextSineValue();

				if (inputSample < 0)
				{
					avgIn = -inputSample;
				}
				else
				{
					avgIn = inputSample;
				}
				avgInOld = ((AVERAGING_LOWPASS_CUTOFF*avgIn) >> 15) + (((32767-AVERAGING_LOWPASS_CUTOFF)*avgInOld) >> 15);

				inputSample = fxPrograms[fxProgramIdx]->processSample(inputSample,fxPrograms[fxProgramIdx]->data);


				if (inputSample < 0)
				{
					avgOut = -inputSample;
				}
				else
				{
					avgOut = inputSample;
				}
				avgOutOld = ((AVERAGING_LOWPASS_CUTOFF*avgOut) >> 15) + (((32767-AVERAGING_LOWPASS_CUTOFF)*avgOutOld) >> 15);

				//carrybit= inputSample & 0x1;
				//wordout = (carrybitOld << 31) | (inputSample << 15) | (0x7FFF & (inputSample >> 1)) ;
				//carrybitOld = carrybit; 
				*((uint32_t*)audioBufferPtr+c) = ((uint16_t)inputSample << 16) | (0xFFFF & (uint16_t)inputSample); //wordout; 

			}
			task &= ~((1 << TASK_PROCESS_AUDIO) | (1 << TASK_PROCESS_AUDIO_INPUT));
			bufferCnt++;

			ticEnd = getTimeLW();
			if(ticEnd > ticStart)
			{
				cpuLoad = ticEnd-ticStart;
				cpuLoad = cpuLoad*196; //*256*256*F_SAMPLING/AUDIO_BUFFER_SIZE/1000000;
				cpuLoad = cpuLoad >> 8;
			}
		}
		if (bufferCnt == UI_UPDATE_IN_SAMPLE_BUFFERS)
		{
			bufferCnt = 0;
			task |= (1 << TASK_UPDATE_AUDIO_UI);
		}
		
	}
}
#endif
#endif
#endif
