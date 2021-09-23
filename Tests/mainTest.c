#include <stdio.h>
#include "consoleHandler.h"
#include "taskManager.h"
#include "system.h"

int main(int argc,char** argv)
{
	RGBStream lamps[N_LAMPS];

	onCharacterReception((uint8_t)'B',lamps);
	handleHelp(0,0);
	printf("done");
	return 0;
}

