#include <stdint.h>
#include "audio/fxprogram/fxProgram.h"
#include "stringFunctions.h"

static int16_t fxProgram2processSample(int16_t sampleIn,void*data)
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    sampleIn >>= 1;
    return simpleChorusProcessSample(sampleIn,&pData->chorusData);
}

static void fxProgram2Param1Callback(uint16_t val,void*data) // frequency
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    // map 0 - 4095 to 1 1000
    val = ((val*250) >> 10) + 1;
    simpleChorusSetFrequency(val,&pData->chorusData);
}

static void fxProgram2Param1Display(void*data,char*res)
{
    FxProgramType * fData = (FxProgramType*)data;
    FxProgram2DataType* pData = (FxProgram2DataType*)fData;
    decimalInt16ToChar(pData->chorusData.frequency,res,2);
    for (uint8_t c=0;c<PARAMETER_NAME_MAXLEN-3;c++)
    {
        if(*(res+c)==0)
        {
            *(res+c)=' ';
            *(res+c+1)='H';
            *(res+c+2)='z';
            *(res+c+3)=(char)0;
            break;
        }
    }
}

static void fxProgram2Param2Callback(uint16_t val,void*data) // depth
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    // map to 0 to 255
    val >>= 4;
    pData->chorusData.depth = (uint8_t)val;
}

static void fxProgram2Param2Display(void*data,char*res)
{
    int16_t dVal;
    FxProgramType * fData = (FxProgramType*)data;
    dVal = fData->parameters[1].rawValue/328;
    Int16ToChar(dVal,res);
    for (uint8_t c=0;c<PARAMETER_NAME_MAXLEN-1;c++)
    {
        if(*(res+c)==0)
        {
            *(res+c)='%';
            *(res+c+1)=(char)0;
            break;
        }
    }
}

static void fxProgram2Param3Callback(uint16_t val,void*data) // mix
{
    FxProgram2DataType* pData = (FxProgram2DataType*)data;
    // map to 0 to 255
    val >>= 4;
    pData->chorusData.mix = (uint8_t)val;
}

static void fxProgram2Param3Display(void*data,char*res)
{
    int16_t dVal;
    FxProgramType * fData = (FxProgramType*)data;
    dVal = fData->parameters[2].rawValue/328;
    Int16ToChar(dVal,res);
    for (uint8_t c=0;c<PARAMETER_NAME_MAXLEN-1;c++)
    {
        if(*(res+c)==0)
        {
            *(res+c)='%';
            *(res+c+1)=(char)0;
            break;
        }
    }
}

static void fxProgram2Setup(void*data)
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
    .name = "Vibrato/Chorus       ",
    .nParameters=3,
    .processSample = &fxProgram2processSample,
    .parameters = {
        {
            .name = "Frequency      ",
            .control=0,
            .minValue=0,
            .maxValue=32767,
            .rawValue=0,
            .getParameterDisplay=&fxProgram2Param1Display,
            .getParameterValue=0,
            .setParameter=&fxProgram2Param1Callback
        },
        {
            .name = "Depth          ",
            .control=1,
            .minValue=0,
            .maxValue=32767,
            .rawValue=0,
            .getParameterDisplay=&fxProgram2Param2Display,
            .getParameterValue=0,
            .setParameter=&fxProgram2Param2Callback
        },
        {
            .name = "Mix            ",
            .control=2,
            .minValue=0,
            .maxValue=32767,
            .rawValue=0,
            .getParameterDisplay=&fxProgram2Param3Display,
            .getParameterValue=0,
            .setParameter=&fxProgram2Param3Callback
        }
    },
    .setup = &fxProgram2Setup,
    .data = (void*)&fxProgram2data
};
