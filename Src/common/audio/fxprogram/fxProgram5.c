#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram5processSample(int16_t sampleIn,void*data)
{
    FxProgram5DataType* pData= (FxProgram5DataType*)data;
    return bitCrusherProcessSample(sampleIn,&pData->bitcrusher);
}

void fxProgram5Param1Callback(uint16_t val,void*data) // set bit mask
{
    FxProgram5DataType* pData= (FxProgram5DataType*)data;
    uint16_t resolution;
    resolution = (4096 - val) >> 8;
    setBitMask((uint8_t)resolution,&pData->bitcrusher);
}

void fxProgram5Param2Callback(uint16_t val,void*data)
{

}

void fxProgram5Param3Callback(uint16_t val,void*data)
{

}

void fxProgram5Setup(void*data)
{

}

FxProgram5DataType fxProgram5data = {
    .bitcrusher = {
        .bitmask = 0x8000
    }
};

FxProgramType fxProgram5 = {
    .name = "Bitcrusher          ",
    .processSample = &fxProgram5processSample,
    .param1Callback = &fxProgram5Param1Callback,
    .param2Callback = &fxProgram5Param2Callback,
    .param3Callback = &fxProgram5Param3Callback,
    .setup = &fxProgram5Setup,
    .data = (void*)&fxProgram5data
};