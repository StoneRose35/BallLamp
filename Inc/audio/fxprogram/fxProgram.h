#ifndef _FX_PROGRAM_H_
#define _FX_PROGRAM_H_
#include <stdint.h>
#include "audio/waveShaper.h"
#include "audio/secondOrderIirFilter.h"
#include "audio/firFilter.h"
#include "audio/simpleChorus.h"

#define N_FX_PROGRAMS 2

typedef int16_t(*processSampleCallback)(int16_t,void*);
typedef void(*paramChangeCallback)(uint16_t,void*);
typedef void(*setupCallback)(void*);

typedef struct {
    char name[24];
    paramChangeCallback param1Callback;
    paramChangeCallback param2Callback;
    paramChangeCallback param3Callback;
    processSampleCallback processSample;
    setupCallback setup;
    void * data;
} FxProgramType;

typedef struct {
    int16_t highpassCutoff;
    uint8_t nWaveshapers;
    uint8_t cabSimOnOff;
    int16_t highpass_out,highpass_old_out,highpass_old_in;
    WaveShaperDataType waveshaper1;
    FirFilterType filter3;
    SecondOrderIirFilterType filter1;
    uint8_t updateLock;
} FxProgram1DataType;

typedef struct {
    SimpleChorusType chorusData;
} FxProgram2DataType;

FxProgramType fxProgram1;
FxProgramType fxProgram2;

FxProgramType * fxPrograms[N_FX_PROGRAMS];

#endif