#include "system.h"
#include "dma.h"
#include "multicore.h"
#include "core1Main.h"
#include "ssd1306_display.h"
#include "audio/firFilter.h"
#include "audio/fxprogram/fxProgram.h"
#include "adc.h"
#include "rotaryEncoder.h"
#include "i2s.h"

int16_t firstHalfOut;
FirFilterType**core1FirData;
extern volatile uint32_t task;
extern volatile int16_t avgOutOld,avgInOld;
extern volatile uint8_t fxProgramIdx;
extern volatile uint32_t cpuLoad;
extern volatile uint32_t audioState;
int16_t avgOldOutBfr;
int16_t avgOldInBfr;
uint8_t cpuLoadBfr;
uint8_t bargraphBuffer[128];
uint8_t switchValOld=0, switchVal=0;
uint16_t adcChannelOld=0,adcChannel=0;
#define UI_DMIN 1

void isr_sio_irq_proc1_irq16() // only fires when a fir computation has to be made
{
    if ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB))!= 0)
    {
        core1FirData = (FirFilterType**)*SIO_FIFO_RD;
        firstHalfOut = processFirstHalf(*core1FirData);
        *SIO_FIFO_WR = firstHalfOut;
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
    *NVIC_ISER = (1 << 16); //enable interrupt for adc and sio of proc1 
    for(;;)
    {
        if ((task & (1 << TASK_UPDATE_POTENTIOMETER_VALUES)) == (1 << TASK_UPDATE_POTENTIOMETER_VALUES))
        {
            // call the update function of the chosen program
            adcChannel = getChannel0Value();
            if ((adcChannel > adcChannelOld) && (adcChannel-adcChannelOld) > UI_DMIN )
            {
                fxPrograms[fxProgramIdx]->param1Callback(adcChannel,fxPrograms[fxProgramIdx]->data);
                adcChannelOld=adcChannel;
            }
            else if ((adcChannel < adcChannelOld) && (adcChannelOld-adcChannel) > UI_DMIN )
            {
                fxPrograms[fxProgramIdx]->param1Callback(adcChannel,fxPrograms[fxProgramIdx]->data);
                adcChannelOld=adcChannel;
            }

            adcChannel = getChannel1Value();
            if ((adcChannel > adcChannelOld) && (adcChannel-adcChannelOld) > UI_DMIN )
            {
                fxPrograms[fxProgramIdx]->param2Callback(adcChannel,fxPrograms[fxProgramIdx]->data);
                adcChannelOld=adcChannel;
            }
            else if ((adcChannel < adcChannelOld) && (adcChannelOld-adcChannel) > UI_DMIN )
            {
                fxPrograms[fxProgramIdx]->param2Callback(adcChannel,fxPrograms[fxProgramIdx]->data);
                adcChannelOld=adcChannel;
            }

            adcChannel = getChannel2Value();
            if ((adcChannel > adcChannelOld) && (adcChannel-adcChannelOld) > UI_DMIN )
            {
                fxPrograms[fxProgramIdx]->param3Callback(adcChannel,fxPrograms[fxProgramIdx]->data);
                adcChannelOld=adcChannel;
            }
            else if ((adcChannel < adcChannelOld) && (adcChannelOld-adcChannel) > UI_DMIN )
            {
                fxPrograms[fxProgramIdx]->param3Callback(adcChannel,fxPrograms[fxProgramIdx]->data);
                adcChannelOld=adcChannel;
            }
            task &= ~(1 << TASK_UPDATE_POTENTIOMETER_VALUES);
            //*ADC_CS |= (1 << ADC_CS_START_MANY_LSB);
        }
        if ((task & (1 << TASK_UPDATE_AUDIO_UI)) == (1 << TASK_UPDATE_AUDIO_UI))
        {
            avgOldInBfr = avgInOld >> 8;
            avgOldOutBfr = avgOutOld >> 8;
            cpuLoadBfr = (cpuLoad >> 1);
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

            for (uint8_t c=0;c<128;c++)
            {
                if (c<=cpuLoadBfr)
                {
                    bargraphBuffer[c] = 126;
                }
                else
                {
                    bargraphBuffer[c] = 0;
                }
            }
            ssd1306DisplayByteArray(3,0,bargraphBuffer,128);

            if ((audioState & (1 << AUDIO_STATE_BUFFER_UNDERRUN)) != 0)
            {
                ssd1306WriteText("BUFFER UNDERRUN!!",0,7);
            }
            else
            {
                ssd1306WriteText("                 ",0,7);
            }

            task &= ~(1 << TASK_UPDATE_AUDIO_UI);
        }
        switchVal = getSwitchValue();
        if (switchVal == 0 && switchValOld == 1)
        {
            fxProgramIdx++;
            if(fxProgramIdx == N_FX_PROGRAMS)
            {
                fxProgramIdx = 0;
            }
            ssd1306WriteText(fxPrograms[fxProgramIdx]->name,0,0);
        }
        switchValOld = getSwitchValue();
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