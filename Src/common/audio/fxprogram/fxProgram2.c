#include <stdint.h>
#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram2processSample(int16_t sampleIn,void*data)
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    sampleIn >>= 1;
    return simpleChorusProcessSample(sampleIn,&pData->chorusData);
}

void fxProgram2Param1Callback(uint16_t val,void*data) // frequency
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    // map 0 - 4095 to 1 1000
    val = ((val*250) >> 10) + 1;
    simpleChorusSetFrequency(val,&pData->chorusData);
}

void fxProgram2Param2Callback(uint16_t val,void*data) // depth
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    // map to 0 to 255
    val >>= 4;
    pData->chorusData.depth = (uint8_t)val;
}

void fxProgram2Param3Callback(uint16_t val,void*data) // mix
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    // map to 0 to 255
    val >>= 4;
    pData->chorusData.mix = (uint8_t)val;
}

void fxProgram2Setup(void*data)
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    initSimpleChorus(&pData->chorusData);
}

FxProgram2DataType fxProgram2data = {
    .chorusData = {
        .mix = 128,
        .frequency = 500,
        .depth = 10
    }
};

FxProgramType fxProgram2 = {
    .name = "Vibrato/Chorus      ",
    .processSample = &fxProgram2processSample,
    .param1Callback = &fxProgram2Param1Callback,
    .param1Name = "Frequency      ",
    .param2Callback = &fxProgram2Param2Callback,
    .param2Name = "Depth          ",
    .param3Callback = &fxProgram2Param3Callback,
    .param3Name = "Mix            ",
    .setup = &fxProgram2Setup,
    .data = (void*)&fxProgram2data
};
