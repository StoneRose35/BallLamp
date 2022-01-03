
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <utilCommands.h>
#include <bufferedInputHandler.h>
#include "neopixelCommands.h"
#include "bluetoothATConfig.h"
#include "systick.h"
#include "stringFunctions.h"
#include "neopixelDriver.h"
#include "uart.h"

/**
 * @brief prints a brief overview of the commands
 * 
 * @param cmd the command, doesn't take any arguments
 * @param context unused
 */
void helpCommand(char * cmd,void* context)
{
	printf("Supported Color commands are\r\n");
	printColorCommands();
	printf(" * RGB(<red>,<green>,<blue>,<LedNumbers>)\r\n");
	printf("   LedNumbers can be gives as any comma-separated list of lamp or ranges of lamps\r\n");
	printf("   a range of lamps is defined as <lower>-<upper>, example: '3-6' would be\r\n");
	printf("   equal to writing 3,4,5,6\r\n");
	printf("   <red>, <green> and <blue> range from 0 to 255\r\n");
	printf("   example: RED(13) switches led 13 to red while leaving all others\r\n");
	printf(" * NPENGINE(<f>): Starts(<f> = 1) or Stops (<f> = 0) the neopixel engine, if stopped issuing color commands has no effect\r\n");
	printf(" * START: starts all interpolators\r\n");
	printf(" * STOP: stops all interpolators\r\n");
	printf(" * INTERP(<lamp_nr>,<nSteps>,<repeating>): defines an interpolator/color sequence for Lamp <lampnr>, 255 means every lamp\r\n");
	printf("   with <nSteps> steps, loops if <repeating> is 1, executes as one-shot if 0 \r\n");
	printf(" * ISTEP(<r>,<g>,<b>,<frames>,<interpolation>,<lampnr>,<step>)\r\n");
	printf("   with <r>, <g>, <b> being the colors, <frames> the number of frames the step should last\r\n");
	printf("   <interpolation> is 0 for constant, 1 for linear interpolation to the next value\r\n");
	printf("   <lampnr> is the lamp nr, 255 means every lamp,<step> is the step to define\r\n");
	printf("   make sure to call INTERP prior to defining individual steps\r\n");
	printf(" * DESCI: returns a description of all initialized interpolators\r\n");
	printf(" * DESTROY(<lampnr>) removes an interpolator for a certain lamp nr or all lamps if 255 is give as a lamp nr\r\n");
	printf("   does nothing if the interpolator is not initialized\r\n");
	printf(" * SAVE: persistently saves the interpolators\r\n");
	printf(" * LOAD: loads the interpolators\r\n");
	printf(" * API: switches physical interface to api mode, does not echo character and does not allow \r\n");
	printf("   command line editing and history\r\n");
	printf(" * CONSOLE: switches the physical interface to console mode, characters received are echoed\r\n");
	printf("   also command line history and editing is possible\r\n");
	printf(" * SETUPBT: changes the bluetooth interface name and pin, sets bluetooth speed to 57600\r\n");
	printf(" * SYSINFO: displays system and memory infos\r\n");

}



/**
 * @brief switches the mode of the interface to API. Nothing happens if the mode is already API.
 * 
 * Note: it is only possible to change the mode of the interface which is accessed.
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the BufferedInput from which the command has been sent.
 */
void apiCommand(char * cmd,void* context)
{
	BufferedInput binput = (BufferedInput)context;
	binput->interfaceType = BINPUT_TYPE_API;
	printf("APIOK\n");
	binput->console->outBfr[0]=0; // disable command prompt already generated by the console handler
}

/**
 * @brief switches the mode of the interface to Console/CLI. Nothing happens if the mode is already Console/CLI.
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context the BufferedInput from which the command has been sent.
 */
void consoleCommand(char * cmd,void* context)
{
	BufferedInput binput = (BufferedInput)context;
	binput->interfaceType = BINPUT_TYPE_CONSOLE;
	printf("Switching back to Console Mode\r\n");
}

/**
 * @brief sets up Bluetooth, Deprecated.
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context unused
 */
void setupBluetoothCommand(char * cmd,void* context)
{
	if(ATCheckEnabled() == 0)
	{
		ATSetName("BallLamp");
		ATSetPin("3006");
		ATSetBaud("7");
	}
	else
	{
		printf("Can't configure Bluetooth: already paired\r\n");
	}
}

/**
 * @brief prints out system information
 * 
 * @param cmd the command, doesn't taken any arguments, therefore unused here.
 * @param context unused
 */
void sysInfoCommand(char * cmd,void*context)
{
	uint32_t current_sp = (uint32_t)address;
	struct mallinfo heapInfo;
	uint32_t nticks;
	uint8_t npEngineState;
	char nrbfr[16];
	heapInfo = mallinfo();
	nticks = getTickValue();
	printf("\r\n\r\nTicks since Start: ");
	UInt32ToChar(nticks,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Current Stack Pointer Location (bytes): ");
	UInt32ToHex(current_sp,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Total Allocated Space (bytes): ");
	UInt32ToChar((uint32_t)heapInfo.uordblks,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Total Free Space (bytes): ");
	UInt32ToChar((uint32_t)heapInfo.fordblks,nrbfr);
	printf(nrbfr);
	printf("\r\n");
	printf("Neopixel Engine State: ");
	npEngineState = getEngineState();
	if(npEngineState == 1)
	{
		printf("On\r\n");
	}
	else
	{
		printf("Off\r\n");
	}
}