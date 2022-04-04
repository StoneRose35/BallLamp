#include <stdint.h>
#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram4processSample(int16_t sampleIn,void*data)
{
    int32_t out;
    FxProgram4DataType* pData = (FxProgram4DataType*)data;

    //pData->highpass_out = (((((1 << 15) + 31000) >> 1)*(sampleIn - pData->highpass_old_in))>>15) + ((31000 *pData->highpass_old_out) >> 15);
    //pData->highpass_old_in = sampleIn;
    //pData->highpass_old_out = pData->highpass_out;

    //out = pData->highpass_out;
    out = sampleIn;
    out = gainStageProcessSample(out, &pData->gainStage);

    //out = waveShaperProcessSample(out,&pData->waveshaper1.waveshaper); //  OversamplingDistortionProcessSample(out,&pData->waveshaper1);

    //out = out >> 2;
    if (pData->cabSimOnOff == 1)
    {
        out = secondOrderIirFilterProcessSample(out,&pData->filter1);
    }
    else if (pData->cabSimOnOff == 2)
    {
        out = secondOrderIirFilterProcessSample(out,&pData->filter1);
        out = secondOrderIirFilterProcessSample(out,&pData->filter2);
    }
    else if (pData->cabSimOnOff == 3)
    {
        out = secondOrderIirFilterProcessSample(out,&pData->filter1);
        out = secondOrderIirFilterProcessSample(out,&pData->filter2);
        out = secondOrderIirFilterProcessSample(out,&pData->filter3);
    }
    return out;
}


void fxProgram4Param1Callback(uint16_t val,void*data) // gain
{
    FxProgram4DataType* pData = (FxProgram4DataType*)data;

    pData->gainStage.gain=(val & (15 << 8))<<3;
}

void fxProgram4Param2Callback(uint16_t val,void*data) // offset
{
    FxProgram4DataType* pData = (FxProgram4DataType*)data;
    pData->gainStage.offset = val;//(val << 4) - 0x7FFF; 
}


void fxProgram4Param3Callback(uint16_t val,void*data) // choose cab sim filters
{

    FxProgram4DataType* pData = (FxProgram4DataType*)data;
    pData->cabSimOnOff = (val >> 10) & 3;
}

void fxProgram4Setup(void*data)
{
    FxProgram4DataType* pData = (FxProgram4DataType*)data;
    initOversamplingWaveshaper(&pData->waveshaper1);
    initWaveShaper(&pData->waveshaper1.waveshaper,&waveShaperDistortion);
    //initWaveShaper(&pData->waveshaper1,&waveShaperDefaultOverdrive);
}

FxProgram4DataType fxProgram4data = {
    /* butterworth lowpass @ 6000Hz */
    .filter1 = {
        	.coeffB = {3199, 6398, 3199},
            .coeffA = {-30893, 10922},
            .w= {0,0,0},
            .bitRes= 16
    },
    /*butterworth highpass @170Hz*/
    .filter2 = {
        .coeffB = {1007, -2014, 1007},
        .coeffA = {-2013,991},
        .w= {0,0,0},
        .bitRes = 11 
    },/*
    .filter2 = { // Hi-Res
        .coeffB = { 32255, -64510, 32255},
        .coeffA = {-64502, 31751},
        .w = {0,0,0},
        .bitRes=16
    },*/
    /*Chebychev type 1 notch filter from 100 to 700 Hz*/
    
    .filter3 = {
        .coeffB = {1831, -3660, 1831},
        .coeffA = {-3660, 1615},
        .w= {0,0,0},
        .bitRes=12 
    },/*
        .filter3 = { // Hi-Res
        .coeffB = { 29311,-58588, 29311},
        .coeffA = {-58588, 25856},
        .w= {0,0,0},
        .bitRes=16 
    },*/
    .highpass_old_in=0,
    .highpass_old_out=0,
    .highpass_out=0,
    .gainStage.gain=512,
    .cabSimOnOff = 1
};

FxProgramType fxProgram4 = {
    .name = "Amp Hi-Gain        ",
    .processSample = &fxProgram4processSample,
    .param1Callback = &fxProgram4Param1Callback,
    .param2Callback = &fxProgram4Param2Callback,
    .param3Callback = &fxProgram4Param3Callback,
    .setup = &fxProgram4Setup,
    .data = (void*)&fxProgram4data
};