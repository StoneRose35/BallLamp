#include "audio/oversamplingWaveshaper.h"
#include "audio/secondOrderIirFilter.h"
#include <stdint.h>
#include "i2s.h"

// oversampling factor as power of two
#define OVERSAMPLING_FACTOR 2 



void initOversamplingWaveshaper(OversamplingWaveshaperDataType*data)
{
    data->oversamplingFilter1.coeffA[0]=-32767;
    data->oversamplingFilter1.coeffA[1]=15967;
    data->oversamplingFilter1.coeffB[0]=15514;
    data->oversamplingFilter1.coeffB[1]=-21858;
    data->oversamplingFilter1.coeffB[2]=15514;
    data->oversamplingFilter1.w[0]=0;
    data->oversamplingFilter1.w[1]=0;
    data->oversamplingFilter1.w[2]=0;

    data->oversamplingFilter2.coeffA[0]=-8004;
    data->oversamplingFilter2.coeffA[1]=14835;
    data->oversamplingFilter2.coeffB[0]=19741;
    data->oversamplingFilter2.coeffB[1]=115;
    data->oversamplingFilter2.coeffB[2]=19741;
    data->oversamplingFilter2.w[0]=0;
    data->oversamplingFilter2.w[1]=0;
    data->oversamplingFilter2.w[2]=0;

    data->oversamplingFilter2.coeffA[0]=30655;
    data->oversamplingFilter2.coeffA[1]=17529;
    data->oversamplingFilter2.coeffB[0]=23714;
    data->oversamplingFilter2.coeffB[1]=33523;
    data->oversamplingFilter2.coeffB[2]=23714;
    data->oversamplingFilter2.w[0]=0;
    data->oversamplingFilter2.w[1]=0;
    data->oversamplingFilter2.w[2]=0;

    initWaveShaper(&data->waveshaper,&waveShaperDefaultOverdrive);
}

//uint16_t oversampledBuffer[AUDIO_BUFFER_SIZE*2*(1 << OVERSAMPLING_FACTOR)];

void  applyOversamplingDistortion(uint16_t*data,OversamplingWaveshaperDataType* waveshaper)
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
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter3);
        waveShaperProcessSample(oversample,&waveshaper->waveshaper);
        //oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter1);
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter3);

        if ((c&OVERSAMPLING_FACTOR)!=0)
        {
            data[c >> OVERSAMPLING_FACTOR] = oversample;
        }
    }
}
int16_t  OversamplingDistortionProcessSample(int16_t sample,OversamplingWaveshaperDataType* waveshaper)
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
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter3);
        waveShaperProcessSample(oversample,&waveshaper->waveshaper);
        //oversample=secondOrderIirFilterProcessSample(oversample,&oversamplingFilter1);
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter2);
        oversample=secondOrderIirFilterProcessSample(oversample,&waveshaper->oversamplingFilter3);

        //if ((c&OVERSAMPLING_FACTOR)!=0)
        //{
        //    data[c >> OVERSAMPLING_FACTOR] = oversample;
        //}
    }
    return oversample;
}

