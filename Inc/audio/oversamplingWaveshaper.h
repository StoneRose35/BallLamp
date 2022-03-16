#ifndef _OVERSAMPLING_WAVESHAPER_H_
#define _OVERSAMPLING_WAVESHAPER_H_
#include <stdint.h>
#include "audio/waveShaper.h"
void initOversamplingWaveshaper(WaveShaperDataType*data);
void  applyOversamplingDistortion(uint16_t*data,WaveShaperDataType*waveshaper);
int16_t  OversamplingDistortionProcessSample(int16_t sample,WaveShaperDataType* waveshaper);
#endif