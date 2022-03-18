#include "system.h"
#include "dma.h"
#include "multicore.h"
#include "core1Main.h"
#include "ssd1306_display.h"
#include "audio/firFilter.h"
#include "audio/fxprogram/fxProgram.h"
#include "adc.h"

int16_t secondHalfOut;
FirFilterType**core1FirData;
extern volatile uint32_t task;
extern volatile int16_t avgOutOld,avgInOld;
int16_t avgOldOutBfr;
int16_t avgOldInBfr;
uint8_t bargraphBuffer[128];

void isr_sio_irq_proc1_irq16() // only fires when a fir computation has to be made
{
    if ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB))!= 0)
    {
        core1FirData = (FirFilterType**)*SIO_FIFO_RD;
        secondHalfOut = processFirstHalf(*core1FirData);
        *SIO_FIFO_WR = secondHalfOut;
        *SIO_FIFO_ST = (1 << 2);
    }
    else if (((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_ROE_LSB)) != 0) || ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_WOF_LSB)) != 0))
    {
        *SIO_FIFO_ST = (1 << 2);
    }
}

void core1Main()
{

    *SIO_FIFO_ST = (1 << 2);
    *SIO_FIFO_WR=0xcafeface; // write sync word for core 0 to wait for core 1
    *NVIC_ISER =  (1 << 22) | (1 << 16); //enable interrupt for adc and sio of proc1 
    for(;;)
    {
        if ((task & (1 << TASK_UPDATE_POTENTIOMETER_VALUES)) == (1 << TASK_UPDATE_POTENTIOMETER_VALUES))
        {
            // call the update function of the chosen program
            fxProgram1.param1Callback(getChannel0Value(),fxProgram1.data);
            fxProgram1.param2Callback(getChannel1Value(),fxProgram1.data);
            fxProgram1.param3Callback(getChannel2Value(),fxProgram1.data);

            task &= ~(1 << TASK_UPDATE_POTENTIOMETER_VALUES);

        }
        if ((task & (1 << TASK_UPDATE_AUDIO_UI)) == (1 << TASK_UPDATE_AUDIO_UI))
        {
            avgOldInBfr = avgInOld >> 8;
            avgOldOutBfr = avgOutOld >> 8;
            for (uint8_t c=0;c<128;c++)
            {
                if (c<=avgOldInBfr)
                {
                    bargraphBuffer[c] = 126;
                }
                else
                {
                    bargraphBuffer[c] = 0;
                }
            }
            ssd1306DisplayByteArray(1,0,bargraphBuffer,128);
            for (uint8_t c=0;c<128;c++)
            {
                if (c<=avgOldOutBfr)
                {
                    bargraphBuffer[c] = 126;
                }
                else
                {
                    bargraphBuffer[c] = 0;
                }
            }
            ssd1306DisplayByteArray(2,0,bargraphBuffer,128);

            task &= ~(1 << TASK_UPDATE_AUDIO_UI);
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