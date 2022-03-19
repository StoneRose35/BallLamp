#ifndef _OVERSAMPLING_WAVESHAPER_H_
#define _OVERSAMPLING_WAVESHAPER_H_
#include <stdint.h>
#include "audio/waveShaper.h"
#include "audio/secondOrderIirFilter.h"


typedef struct {
    WaveShaperDataType waveshaper;
    SecondOrderIirFilterType oversamplingFilter1;
    SecondOrderIirFilterType oversamplingFilter2;
    SecondOrderIirFilterType oversamplingFilter3;

} OversamplingWaveshaperDataType;

void initOversamplingWaveshaper(OversamplingWaveshaperDataType*data);
void  applyOversamplingDistortion(uint16_t*data,OversamplingWaveshaperDataType*waveshaper);
int16_t  OversamplingDistortionProcessSample(int16_t sample,OversamplingWaveshaperDataType* waveshaper);
#endif