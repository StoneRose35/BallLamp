#include "system.h"
#include "dma.h"
#include "multicore.h"
#include "audio/firFilter.h"
void core1Main()
{
    int16_t secondHalfOut;
    FirFilterType*data;
    *NVIC_ISER = (1 << 9); // enable interrupt for pio1
    for(;;)
    {
        while ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB)) != (1 << SIO_FIFO_ST_VLD_LSB));
        data = (FirFilterType*)*SIO_FIFO_RD;
        secondHalfOut = processSecondHalf(data);
        *SIO_FIFO_WR = secondHalfOut;
    }
}