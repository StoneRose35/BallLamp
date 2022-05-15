#include "system.h"
#include "dma.h"
#include "multicore.h"
#include "core1Main.h"
#include "ssd1306_display.h"
#include "audio/firFilter.h"
#include "audio/fxprogram/fxProgram.h"
#include "pipicofx/pipicofxui.h"
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
uint8_t switchValsOld[2]={0,0};
uint32_t encoderValOld=0,encoderVal;
uint8_t switchVals[2]={0,0};
//PiPicoFxUiType uiControllerData;
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

    encoderValOld=getEncoderValue();
    for(;;)
    {
        if ((task & (1 << TASK_UPDATE_POTENTIOMETER_VALUES)) == (1 << TASK_UPDATE_POTENTIOMETER_VALUES))
        {
            // call the update function of the chosen program
            adcChannel = getChannel0Value();
            if ((adcChannel > adcChannelOld) && (adcChannel-adcChannelOld) > UI_DMIN )
            {
                knob0Callback(adcChannel,&piPicoUiController);
                adcChannelOld=adcChannel;
            }
            else if ((adcChannel < adcChannelOld) && (adcChannelOld-adcChannel) > UI_DMIN )
            {
                knob0Callback(adcChannel,&piPicoUiController);
                adcChannelOld=adcChannel;
            }

            adcChannel = getChannel1Value();
            if ((adcChannel > adcChannelOld) && (adcChannel-adcChannelOld) > UI_DMIN )
            {
                knob1Callback(adcChannel,&piPicoUiController);
                adcChannelOld=adcChannel;
            }
            else if ((adcChannel < adcChannelOld) && (adcChannelOld-adcChannel) > UI_DMIN )
            {
                knob1Callback(adcChannel,&piPicoUiController);
                adcChannelOld=adcChannel;
            }

            adcChannel = getChannel2Value();
            if ((adcChannel > adcChannelOld) && (adcChannel-adcChannelOld) > UI_DMIN )
            {
                knob2Callback(adcChannel,&piPicoUiController);
                adcChannelOld=adcChannel;
            }
            else if ((adcChannel < adcChannelOld) && (adcChannelOld-adcChannel) > UI_DMIN )
            {
                knob2Callback(adcChannel,&piPicoUiController);
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
            /*
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
            */

/*
            if ((audioState & (1 << AUDIO_STATE_BUFFER_UNDERRUN)) != 0)
            {
                ssd1306WriteText("BUFFER UNDERRUN!!",0,7);
            }
            else
            {
                ssd1306WriteText("                 ",0,7);
            }
*/
            updateAudioUi(avgOldInBfr,avgOldOutBfr,cpuLoadBfr,&piPicoUiController);
            task &= ~(1 << TASK_UPDATE_AUDIO_UI);
        }
        switchVals[0] = getSwitchValue(0);
        if (switchVals[0] == 0 && switchValsOld[0] == 1)
        {
            button1Callback(&piPicoUiController);
            /*fxProgramIdx++;
            if(fxProgramIdx == N_FX_PROGRAMS)
            {
                fxProgramIdx = 0;
            }
            ssd1306WriteText(fxPrograms[fxProgramIdx]->name,0,0);
            ssd1306WriteText(fxPrograms[fxProgramIdx]->param1Name,3,4);
            ssd1306WriteText(fxPrograms[fxProgramIdx]->param2Name,3,5);
            ssd1306WriteText(fxPrograms[fxProgramIdx]->param3Name,3,6);
            ssd1306WriteText("                   ",0,7);
            */
        }
        switchValsOld[0] = getSwitchValue(0);

                switchVals[0] = getSwitchValue(0);
        if (switchVals[1] == 0 && switchValsOld[1] == 1)
        {
            button2Callback(&piPicoUiController);
        }
        switchValsOld[1] = getSwitchValue(1);
        /*
        if ((*SIO_FIFO_ST & (1 << SIO_FIFO_ST_VLD_LSB)) == (1 << SIO_FIFO_ST_VLD_LSB))
        {
            data = (FirFilterType**)*SIO_FIFO_RD;
            secondHalfOut = processFirstHalf(*data);
            //secondHalfOut=0;
            *SIO_FIFO_WR = secondHalfOut;
        }
        */
       encoderVal=getEncoderValue();
       if (encoderValOld != encoderVal)
       {
           rotaryCallback(encoderVal,&piPicoUiController);
           encoderValOld=encoderVal;
       }
    }
}