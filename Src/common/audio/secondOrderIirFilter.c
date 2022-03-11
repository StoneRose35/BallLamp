#include "audio/secondOrderIirFilter.h"

void initSecondOrderIirFilter(SecondOrderIirFilterType* data)
{
    data->w[0]=0;
    data->w[1]=0;
    data->w[2]=0;
    data->coeffB[0]=(1<<15);
    data->coeffB[1]=0;
    data->coeffB[2]=0;
    data->coeffA[0]=0;
    data->coeffA[1]=0;
}

int16_t secondOrderIirFilterProcessSample(int16_t sampleIn,SecondOrderIirFilterType*data)
{
    int16_t out;
    data->w[0] = sampleIn;
    data->w[0] += (data->coeffA[0]*data->w[1]) >> 15;
    data->w[0] += (data->coeffA[1]*data->w[2]) >> 15;

    out = (data->coeffB[0]*data->w[0]) >> 15;
    out += (data->coeffB[1]*data->w[1]) >> 15;
    out += (data->coeffB[2]*data->w[2]) >> 15;

    data->w[2] = data->w[1];
    data->w[1] = data->w[0];


    return out;
}