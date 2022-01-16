#include "ds18b20.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/resets.h"

void initPio()
{
    	// enable the PIO0 block
	*RESETS |= (1 << RESETS_RESET_PIO0_LSB);
    *RESETS &= ~(1 << RESETS_RESET_PIO0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_PIO0_LSB)) == 0);
}