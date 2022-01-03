
#include "dma.h"

void initDMA()
{
	// enable the dma block
	*RESETS |= (1 << RESETS_RESET_DMA_LSB);
    *RESETS &= ~(1 << RESETS_RESET_DMA_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_DMA_LSB)) == 0);
}