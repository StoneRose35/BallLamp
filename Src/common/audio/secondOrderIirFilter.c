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
    int32_t out;
    int32_t a_coeff1, a_coeff2;
    data->w[0] = (int32_t)sampleIn;
    data->w[0] -= (((int32_t)data->coeffA[0]*(int32_t)data->w[1]) >> 15);
    data->w[0] -= (((int32_t)data->coeffA[1]*(int32_t)data->w[2]) >> 15);

    out = (((int32_t)data->coeffB[0]*(int32_t)data->w[0]) >> 15) + (((int32_t)data->coeffB[1]*(int32_t)data->w[1]) >> 15) + (((int32_t)data->coeffB[2]*(int32_t)data->w[2]) >> 15);
    data->w[2] = data->w[1];
    data->w[1] = data->w[0];

    return (int16_t)(out & 0xFFFF);
}