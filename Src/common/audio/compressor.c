#include "stdint.h"
#include "audio/compressor.h"
#include "romfunc.h"

int16_t applyGain(int16_t sample,int16_t avgVolume,GainFunctionType*gainFunction)
{
    if (avgVolume < gainFunction->threshhold)
    {
        return sample;
    }
    else if (gainFunction->gainReduction > 4)
    {
        return gainFunction->threshhold;
    }
    else
    {
        if (sample > 0)
        {
            return gainFunction->threshhold + ((sample-gainFunction->threshhold) >> (gainFunction->gainReduction));
        }
        else
        {
            return -gainFunction->threshhold + ((sample+gainFunction->threshhold) >> (gainFunction->gainReduction));
        }
    }
}

void setAttack(int32_t attackInUs,CompressorDataType*data)
{
    float attackFloat, samplesFloat;
    attackFloat = int2float(attackInUs);
    samplesFloat = 32767.0f*20.833f/attackFloat;
    data->attack= (int16_t)float2int(samplesFloat);
}

void setRelease(int32_t releaseInUs,CompressorDataType*data)
{
    float releaseFloat, samplesFloat;
    releaseFloat = int2float(releaseInUs);
    samplesFloat = 32767.0f*20.833f/releaseFloat;
    data->release= (int16_t)float2int(samplesFloat);
}

int16_t compressorProcessSample(int16_t sampleIn,CompressorDataType*data)
{
    int16_t absSample;
    int16_t delta;
    int16_t sampleOut;

    sampleOut = applyGain(sampleIn,data->currentAvg,&data->gainFunction);
    if(sampleOut < 0)
    {
        absSample = -sampleOut;
    }
    else
    {
        absSample = sampleOut;
    }
    delta = absSample - data->currentAvg;
    if (delta < 0)
    {
        if(-delta > data->release)
        {
            data->currentAvg -= data->release;
        }
        else
        {
            data->currentAvg += delta;
        }
        if (data->currentAvg < 0)
        {
            data->currentAvg=0;
        }
    }
    else
    {
        if(delta > data->attack)
        {
            data->currentAvg += data->attack;
        }
        else
        {
            data->currentAvg += delta;
        }
        if (data->currentAvg < 0) // overflow in this case
        {
            data->currentAvg=(1 << 15);
        }
    }
    return sampleOut;
}