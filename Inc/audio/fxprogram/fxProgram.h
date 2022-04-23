#ifndef _FX_PROGRAM_H_
#define _FX_PROGRAM_H_
#include <stdint.h>
#include "audio/waveShaper.h"
#include "audio/secondOrderIirFilter.h"
#include "audio/firFilter.h"
#include "audio/simpleChorus.h"
#include "audio/oversamplingWaveshaper.h"
#include "audio/gainstage.h"
#include "audio/bitcrusher.h"
#include "audio/delay.h"

#define N_FX_PROGRAMS 6


#define FXPROGRAM6_DELAY_TIME_LOWPASS_T 2
typedef int16_t(*processSampleCallback)(int16_t,void*);
typedef void(*paramChangeCallback)(uint16_t,void*);
typedef void(*setupCallback)(void*);
typedef void*(*getParameterValueFct)(void*);
typedef char*(*getParameterDisplayFct)(void*);

typedef struct {
    const char name[16];
    uint8_t control; // 0-2: Potentiometers, 255: no control binding
    int16_t rawValue;
    int16_t maxValue;
    int16_t minValue;
    getParameterValueFct getParameterValue; // returns the converted parameter value, data type depends on the implementation
    getParameterDisplayFct getParameterDisplay; // returns the display value as a string of a Parameter
} FxProgramParameterType;

typedef struct {
    const char name[24];
    const char param1Name[16];
    const char param2Name[16];
    const char param3Name[16];    
    FxProgramParameterType parameters[8];
    paramChangeCallback param1Callback;
    paramChangeCallback param2Callback;
    paramChangeCallback param3Callback;
    processSampleCallback processSample;
    setupCallback setup;
    uint8_t nParameters;
    void * data;
} FxProgramType;


typedef struct {
    int16_t highpassCutoff;
    uint8_t nWaveshapers;
    int16_t highpass_out,highpass_old_out,highpass_old_in;
    WaveShaperDataType waveshaper1;
    FirFilterType filter3;
    SecondOrderIirFilterType filter1;
    DelayDataType * delay;
} FxProgram1DataType;

typedef struct {
    SimpleChorusType chorusData;
} FxProgram2DataType;

typedef struct {
    gainStageData gainStage;
    uint8_t cabSimType;
    uint8_t nWaveshapers;
    int16_t highpass_out,highpass_old_out,highpass_old_in;
    const char cabNames[6][24];
    FirFilterType hiwattFir;
    OversamplingWaveshaperDataType waveshaper1;
    SecondOrderIirFilterType hiwattIir1;
    SecondOrderIirFilterType hiwattIir2;
    SecondOrderIirFilterType hiwattIir3;
    
    FirFilterType frontmanFir;
    SecondOrderIirFilterType frontmanIir1;
    SecondOrderIirFilterType frontmanIir2;
    SecondOrderIirFilterType frontmanIir3;

    FirFilterType voxAC15Fir;
    SecondOrderIirFilterType voxAC15Iir1;
    SecondOrderIirFilterType voxAC15Iir2;
    SecondOrderIirFilterType voxAC15Iir3;
    

    //uint8_t updateLock;
} FxProgram4DataType;

typedef struct 
{
    BitCrusherDataType bitcrusher;
} FxProgram5DataType;

typedef struct 
{
    DelayDataType * delay;
} FxProgram6DataType;


FxProgramType fxProgram1;
FxProgramType fxProgram2;
FxProgramType fxProgram3;
FxProgramType fxProgram4;
FxProgramType fxProgram5;
FxProgramType fxProgram6;

FxProgramType * fxPrograms[N_FX_PROGRAMS];

#endif