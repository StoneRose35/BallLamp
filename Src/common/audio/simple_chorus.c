
#include "audio/simple_chorus.h"

static int16_t delayBuffer[2048];

static volatile SimpleChorusType simpleChorusData;
static volatile uint16_t lfoUpdateCnt=0;

void initSimpleChorus()
{
    for(uint16_t c=0;c<2048;c++)
    {
        delayBuffer[c]=0;
    }
    simpleChorusData.depth=10;
    simpleChorusData.mix=127;
    //simpleChorusData.frequency= 10;
    simpleChorusData.lfoQuadrant=0;
    simpleChorusData.lfoVal=0;
    simpleChorusData.lfoPhaseinc=500*4*256*SIMPLE_CHORUS_LFO_DIVIDER/4800000;
}

/** set frequency in Hz/100 */
void setFrequency(uint16_t freq)
{
    simpleChorusData.lfoPhaseinc=freq*4*256*SIMPLE_CHORUS_LFO_DIVIDER/4800000;
}


int16_t simpleChorusProcessSample(int16_t sampleIn)
{
    int16_t delayPtr;
    int16_t sampleOut;
    lfoUpdateCnt++;
    if (lfoUpdateCnt == SIMPLE_CHORUS_LFO_DIVIDER)
    {
        if (simpleChorusData.lfoQuadrant == 0 )
        {
            simpleChorusData.lfoVal += simpleChorusData.lfoPhaseinc;
            if (simpleChorusData.lfoVal > 255)
            {
                simpleChorusData.lfoVal = 512 - simpleChorusData.lfoVal;
                simpleChorusData.lfoQuadrant += 1;
                simpleChorusData.lfoQuadrant &= 1;
            }
        }
        else
        {
            simpleChorusData.lfoVal -= simpleChorusData.lfoPhaseinc;
            if (simpleChorusData.lfoVal < -255)
            {
                simpleChorusData.lfoVal = -512 - simpleChorusData.lfoVal;
                simpleChorusData.lfoQuadrant += 1;
                simpleChorusData.lfoQuadrant &= 1;
            }
        }
        lfoUpdateCnt=0;
    }

        simpleChorusData.delayInputPtr &= (SIMPLE_CHORUS_DELAY_SIZE-1);

        // compute current index of the delay pointer
        delayPtr = (simpleChorusData.delayInputPtr-1) - (((simpleChorusData.lfoVal+0xFF)*simpleChorusData.depth) >> 8);
        if (delayPtr < 0)
        {
            delayPtr = SIMPLE_CHORUS_DELAY_SIZE + delayPtr;
        }  
        sampleOut = ((sampleIn*(0xFF - simpleChorusData.mix)) >> 8) + (((simpleChorusData.mix*delayBuffer[delayPtr])>>8));
        *(delayBuffer + simpleChorusData.delayInputPtr++)=sampleIn;
        return sampleOut;
}
