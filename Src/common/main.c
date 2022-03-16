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
#include "romfunc.h"
#include "systemClock.h"
#include "systick.h"
#include "datetimeClock.h"
#include "uart.h"
#include "dma.h"
#include "pio.h"
#include "pwm.h"
#include "adc.h"
#include "spi_sdcard_display.h"
#include "debugLed.h"
#include "fatLib.h"
#include "consoleHandler.h"
#include "apiHandler.h"
#include "bufferedInputHandler.h"
#include "colorInterpolator.h"
#include "interpolators.h"
#include "stringFunctions.h"
#include "taskManager.h"
#include "neopixelCommands.h"
#include "charDisplay.h"
#include "ds18b20.h"
#include "remoteSwitch.h"
#include "rotaryEncoder.h"
#include "cliApiTask.h"
#include "uiStack.h"
#include "heater.h"
#include "services/triopsBreederService.h"
#include "services/hibernateService.h"
#include "i2s.h"
#include "audio/sineplayer.h"
#include "audio/simpleChorus.h"
#include "audio/secondOrderIirFilter.h"
#include "audio/firFilter.h"
#include "audio/waveShaper.h"
#include "audio/oversamplingWaveshaper.h"
#include "multicore.h"
#include "core1Main.h"



RGBStream lampsdata[N_LAMPS];
RGBStream * lamps = lampsdata;
//uint32_t clr_cnt = 0;
//uint32_t bit_cnt = 0;

//uint8_t rawdata[N_LAMPS*24+1];
//uint8_t* rawdata_ptr = rawdata;

//TaskType interpolatorsArray[N_LAMPS];
//TasksType interpolators;

//BufferedInputType btInput;

volatile uint32_t task;
volatile uint8_t context;

extern CommBufferType usbCommBuffer;
extern CommBufferType btCommBuffer;

DirectoryPointerType * cwd;
DirectoryPointerType * ndir;


int16_t* audioBufferPtr;
uint16_t* audioBufferInputPtr;
int16_t inputSample;
SimpleChorusType chorus1;
SecondOrderIirFilterType filter1, filter2;
WaveShaperDataType waveshaper1;
uint8_t carrybitOld=0,carrybit=0;
uint32_t wordout;
uint32_t core1Handshake;

/**
 * @brief the main entry point, should never exit
 * 
 * @return int by definition but should never return a value
 */
int main(void)
{

	int16_t highpass_old_out=0;
	int16_t highpass_old_in=0;
	int16_t highpass_out=0;


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

	initUsbPll();
	initSystickTimer();
	initDMA();
	//initPwm();
	initPio();
	initGpio();
	//initSpi();
	initDatetimeClock();
	initUart(BAUD_RATE);
	initAdc();



	/*
	 *
	 * Initialise Component-specific drivers
	 * 
	 * */
	initCliApi();
	//mountFat32SDCard(&cwd,&ndir);
	//initDisplay();
	//initNeopixels();
	//setEngineState(0);
	//initRemoteSwitch();
	//initRotaryEncoder();
	//initHeater();
	//initDs18b20();
	initI2S();
	initDebugLed();
	


	/*
     *
     * Initialize Background Services
     *
	 */
	initDoubleBufferedReading(0);
	enableAudioEngine();
	enableAudioInput(0);


	/*
	 *
	 * Initialize the Ui Application Layer
	 * 
	 * */


	printf("Microsys v1.0 running\r\n");

	//uint8_t notecnt=0;
	//uint16_t notelength=0;

	//setNote(notecnt);
	initSimpleChorus(&chorus1);
	initSecondOrderIirFilter(&filter1);
	initSecondOrderIirFilter(&filter2);
	initWaveShaper(&waveshaper1,&waveShaperDefaultOverdrive);



	/* filters of the reduced cab simulator, in the order of processing */

	/* butterworth lowpass @ 6000Hz */
	
	filter1.coeffB[0]=3199;
	filter1.coeffB[1]=6398;
	filter1.coeffB[2]=3199;
	filter1.coeffA[0]=-30893;
	filter1.coeffA[1]=10922;
    
	FirFilterType filter3 = {
		.coefficients = {0xd92, 0xe31, 0x113d, 0x19e0, 0x29f1, 0x436b, 0x659b, 0x8242, 0x80ea, 0x5bcf, 0x304b, 0x12a0, 0xf1bd, 0xd3db, 0xcb60, 0xcdf2, 0xd476, 0xe539, 0xf98f, 0x6c2, 0xca4, 0xf38, 0xcde, 0x9f6, 0xd60, 0x105f, 0x1211, 0x1087, 0x5b7, 0xf86c, 0xf2bb, 0xf6d4, 0x5f, 0x973, 0xc4f, 0x67c, 0x94, 0x5, 0xfc61, 0xf7b8, 0xfa0f, 0x193, 0xa46, 0x11e5, 0x1681, 0x16e9, 0x13f9, 0x100c, 0xe8a, 0xf2a, 0xfef, 0xef4, 0xa70, 0x494, 0xff4a, 0xfabc, 0xfb5e, 0xff9b, 0x4b2, 0x85c, 0xb1c, 0xd25, 0xdf6, 0xd76}
	};

	initfirFilter(&filter3);
	/* chebychev highpass @ 100Hz,bandstop 20dB */
	filter2.coeffB[0]=32638;
	filter2.coeffB[1]=259;
	filter2.coeffB[2]=32638;
	filter2.coeffA[0]=260;
	filter2.coeffA[1]=32510;

	// sync with core 1
	while ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB)) != (1 << SIO_FIFO_ST_VLD_LSB));
	core1Handshake=*SIO_FIFO_RD;
	while (core1Handshake != 0xcafeface)
	{
		DebugLedOn();
		core1Handshake = *SIO_FIFO_RD;
	}

    /* Loop forever */
	for(;;)
	{

		cliApiTask(task);
		if ((task & (1 << TASK_PROCESS_AUDIO))!= 0)
		{
			audioBufferPtr = getEditableAudioBuffer();
			audioBufferInputPtr = getReadableAudioBuffer();
			for (uint8_t c=0;c<AUDIO_BUFFER_SIZE;c++)
			{
				// convert raw input to signed 16 bit
				inputSample = (*(audioBufferInputPtr + c) << 4) - 0x7FFF;
				inputSample = inputSample >> 1;
				// high-pass the input to remove dc component
				
				#define ALPHA 31000
				//highpass_out = ((ALPHA*highpass_old_out) >> 15) + (((inputSample - highpass_old_out)*((1 << 15) - ALPHA)) >> 15);
				highpass_out = (((((1 << 15) + ALPHA) >> 1)*(inputSample - highpass_old_in))>>15) + ((ALPHA*highpass_old_out) >> 15);
				highpass_old_in = inputSample;
				highpass_old_out = highpass_out;

				inputSample = highpass_out;
				//inputSample = simpleChorusProcessSample(inputSample,&chorus1);
				//inputSample = waveShaperProcessSample(inputSample,&waveshaper1);
				inputSample = OversamplingDistortionProcessSample(inputSample,&waveshaper1);

				inputSample = secondOrderIirFilterProcessSample(inputSample,&filter1);
				inputSample = firFilterProcessSample(inputSample,&filter3);
				//inputSample = secondOrderIirFilterProcessSample(inputSample,&filter2);
				


				carrybit= inputSample & 0x1;
				wordout = (carrybitOld << 31) | (inputSample << 15) | (0x7FFF & (inputSample >> 1)) ;
				carrybitOld = carrybit; 
				*((uint32_t*)audioBufferPtr+c) = wordout; //getNextSineValue(); // ((highpass_out >> 2)*((getNextSineValue()>>3) + (1 << 14))) >> 15;

			}
			task &= ~((1 << TASK_PROCESS_AUDIO) | (1 << TASK_PROCESS_AUDIO_INPUT));
		}
		

	// test the rom functions
	/*
		BootRomInfoType*  bootInfo;
		getBootRomInfo(&bootInfo);
		char * copyrightNote;
		copyrightNote = getCopyright();


		c = fadd(a,b);
		c = fsub(c,9.8);
		c = fmul(c,a);
		c = fdiv(c,b);
		printf(copyrightNote);
	*/	
	}
}
#endif
#endif
#endif
