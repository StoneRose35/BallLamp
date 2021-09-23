#include <stdio.h>
#include "consoleHandler.h"
#include "taskManager.h"
#include "system.h"

int main(int argc,char** argv)
{
	int state = 0;
	RGBStream lamps[N_LAMPS];

	onCharacterReception((uint8_t)'B',lamps);
	onCharacterReception((uint8_t)'L',lamps);
	onCharacterReception((uint8_t)'U',lamps);
	onCharacterReception((uint8_t)'E',lamps);
	onCharacterReception((uint8_t)'(',lamps);
	onCharacterReception((uint8_t)'0',lamps);
	onCharacterReception((uint8_t)')',lamps);
	onCharacterReception((uint8_t)'\r',lamps);
	state = 1;
	onCharacterReception((uint8_t)'\e',lamps);
	onCharacterReception((uint8_t)'[',lamps);
	onCharacterReception((uint8_t)'A',lamps);
    state = 2;
	onCharacterReception(127,lamps);
	onCharacterReception(127,lamps);
	onCharacterReception(127,lamps);
	onCharacterReception((uint8_t)'1',lamps);
	state = 3;
	onCharacterReception((uint8_t)'\r',lamps);
	state = 4;
	handleHelp(0,0);
	printf("done");
	return 0;
}

