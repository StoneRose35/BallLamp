#ifndef _WAVESHAPER_H_
#define _WAVESHAPER_H_
#include <stdint.h>

typedef struct 
{
    int16_t transferFunctionPoints[128];

} WaveShaperDataType;


void initWaveShaper(WaveShaperDataType * data);

int16_t waveShaperProcessSample(int16_t sampleIn,WaveShaperDataType*data);
#endif