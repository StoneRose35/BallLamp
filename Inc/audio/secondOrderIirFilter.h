#ifndef _SECOND_ORDER_IIR_FILTER_H_
#define _SECOND_ORDER_IIR_FILTER_H_
#include <stdint.h>
typedef struct 
{
    int16_t w[3];
    int16_t coeffA[2];
    int16_t coeffB[3];
} SecondOrderIirFilterType;

void initSecondOrderIirFilter(SecondOrderIirFilterType* data);

int16_t secondOrderIirFilterProcessSample(int16_t sampleIn,SecondOrderIirFilterType*data);
#endif