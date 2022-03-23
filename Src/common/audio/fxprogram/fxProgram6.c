#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram6processSample(int16_t sampleIn,void*data)
{
    FxProgram6DataType* pData= (FxProgram6DataType*)data;
    return delayLineProcessSample(sampleIn,&pData->delay);
}

void fxProgram6Param1Callback(uint16_t val,void*data) // Delay Time
{
    FxProgram6DataType* pData= (FxProgram6DataType*)data;
    int32_t wVal;
    wVal = val;
    wVal <<= 4;
    pData->delay.delayInSamples=wVal;
}

void fxProgram6Param2Callback(uint16_t val,void*data) // Feedback
{
    FxProgram6DataType* pData= (FxProgram6DataType*)data;
    uint32_t wVal;
    wVal = val;
    wVal <<= 3;
    pData->delay.feedback=(int16_t)wVal;
}


void fxProgram6Param3Callback(uint16_t val,void*data) // Mix
{
    FxProgram6DataType* pData= (FxProgram6DataType*)data;
    int16_t wVal;
    wVal = val;
    wVal <<= 3;
    pData->delay.mix = wVal;
}

void fxProgram6Setup(void*data)
{
    FxProgram6DataType* pData= (FxProgram6DataType*)data;
    initDelay(&pData->delay);
}

FxProgram6DataType fxProgram6data;

FxProgramType fxProgram6 = {
    .name = "Delay               ",
    .processSample = &fxProgram6processSample,
    .param1Callback = &fxProgram6Param1Callback,
    .param2Callback = &fxProgram6Param2Callback,
    .param3Callback = &fxProgram6Param3Callback,
    .setup = &fxProgram6Setup,
    .data = (void*)&fxProgram6data
};