#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram3processSample(int16_t sampleIn,void*data)
{
    return sampleIn;
}

void fxProgram3Param1Callback(uint16_t val,void*data) // frequency
{

}

void fxProgram3Param2Callback(uint16_t val,void*data) // depth
{

}

void fxProgram3Param3Callback(uint16_t val,void*data) // mix
{

}

void fxProgram3Setup(void*data)
{

}

FxProgramType fxProgram3 = {
    .name = "Off                 ",
    .processSample = &fxProgram3processSample,
    .param1Callback = &fxProgram3Param1Callback,
    .param2Callback = &fxProgram3Param2Callback,
    .param3Callback = &fxProgram3Param3Callback,
    .setup = &fxProgram3Setup,
    .data = (void*)0
};
