#include "system.h"
#include "dma.h"
#include "multicore.h"
#include "core1Main.h"
#include "audio/firFilter.h"
#include "audio/fxprogram/fxProgram.h"
#include "adc.h"

int16_t secondHalfOut;
FirFilterType**data;
extern uint32_t task;
extern 
void isr_sio_irq_proc1_irq16() // only fires when a fir computation has to be made
{
    data = (FirFilterType**)*SIO_FIFO_RD;
    secondHalfOut = processFirstHalf(*data);
    *SIO_FIFO_WR = secondHalfOut;
}

void core1Main()
{

    *NVIC_ISER =  (1 << 22) | (1 << 16); //enable interrupt for adc and sio of proc1 
    *SIO_FIFO_WR=0xcafeface; // write sync word for core 0 to wait for core 1
    for(;;)
    {
        if ((task & (1 << TASK_UPDATE_POTENTIOMETER_VALUES)) != 0)
        {
            // call the update function of the chosen program
            fxProgram1.param1Callback(getChannel0Value(),fxProgram1.data);
            fxProgram1.param2Callback(getChannel1Value(),fxProgram1.data);
            fxProgram1.param3Callback(getChannel2Value(),fxProgram1.data);

            task &= ~(1 << TASK_UPDATE_POTENTIOMETER_VALUES);

        }
        /*
        if ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB)) == (1 << SIO_FIFO_ST_VLD_LSB))
        {
            data = (FirFilterType**)*SIO_FIFO_RD;
            secondHalfOut = processFirstHalf(*data);
            //secondHalfOut=0;
            *SIO_FIFO_WR = secondHalfOut;
        }
        */
    }
}