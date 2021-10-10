#include <stdio.h>
#include "consoleHandler.h"
#include "taskManager.h"
#include "system.h"


void consoleHandlerHistoryCheck()
{

	RGBStream lamps[N_LAMPS];

	onCharacterReception((uint8_t)'B',lamps);
	onCharacterReception((uint8_t)'L',lamps);
	onCharacterReception((uint8_t)'U',lamps);
	onCharacterReception((uint8_t)'E',lamps);
	onCharacterReception((uint8_t)'(',lamps);
	onCharacterReception((uint8_t)'0',lamps);
	onCharacterReception((uint8_t)')',lamps);
	onCharacterReception((uint8_t)'\r',lamps);

	onCharacterReception((uint8_t)'\e',lamps);
	onCharacterReception((uint8_t)'[',lamps);
	onCharacterReception((uint8_t)'A',lamps);

	onCharacterReception(127,lamps);
	onCharacterReception(127,lamps);
	onCharacterReception(127,lamps);
	onCharacterReception((uint8_t)'1',lamps);

	onCharacterReception((uint8_t)'\r',lamps);

	handleHelp(0,0);
	printf("done");
}

int main(int argc,char** argv)
{
	RGBStream lamps[N_LAMPS];
	char command[32] = "RGB(23,34,45,1)";
	handleCommand(command,lamps);
}




