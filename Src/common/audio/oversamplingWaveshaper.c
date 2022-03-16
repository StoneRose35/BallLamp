#include "audio/waveShaper.h"
#include "audio/secondOrderIirFilter.h"
#include <stdint.h>
#include "i2s.h"

// oversampling factor as power of two
#define OVERSAMPLING_FACTOR 2 

SecondOrderIirFilterType oversamplingFilter1= {
    .coeffA={-41344, 20147},
    .coeffB={19575, -27580, 19575},
    .w={0,0,0}
};

SecondOrderIirFilterType oversamplingFilter2= {
    .coeffA={-8004,14835},
    .coeffB={19741,115,19741},
    .w={0,0,0}
};

SecondOrderIirFilterType oversamplingFilter3= {
    .coeffA={30655,17529},
    .coeffB={23714,33523,23714},
    .w={0,0,0}
};


void initOversamplingWaveshaper(WaveShaperDataType*data)
{
    initWaveShaper(data,&waveShaperDefaultOverdrive);
}

//uint16_t oversampledBuffer[AUDIO_BUFFER_SIZE*2*(1 << OVERSAMPLING_FACTOR)];

void  applyOversamplingDistortion(uint16_t*data,WaveShaperDataType* waveshaper)
{
    int16_t oversample;
    for (uint16_t c=0;c<AUDIO_BUFFER_SIZE*2*(1 << OVERSAMPLING_FACTOR);c++)
    {
        if ((c&OVERSAMPLING_FACTOR)!=0)
        {
            oversample=data[c >> OVERSAMPLING_FACTOR];
        }
        else
        {
            oversample=0;
        }
        //oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter1);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter3);
        waveShaperProcessSample(oversample,waveshaper);
        //oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter1);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter3);

        if ((c&OVERSAMPLING_FACTOR)!=0)
        {
            data[c >> OVERSAMPLING_FACTOR] = oversample;
        }
    }
}
int16_t  OversamplingDistortionProcessSample(int16_t sample,WaveShaperDataType* waveshaper)
{
    int16_t oversample;
    for (uint16_t c=0;c<(1 << OVERSAMPLING_FACTOR);c++)
    {
        if (c==0)
        {
            oversample=sample;
        }
        else
        {
            oversample=0;
        }
        //oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter1);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter3);
        waveShaperProcessSample(oversample,waveshaper);
        //oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter1);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter3);

        //if ((c&OVERSAMPLING_FACTOR)!=0)
        //{
        //    data[c >> OVERSAMPLING_FACTOR] = oversample;
        //}
    }
    return oversample;
}

