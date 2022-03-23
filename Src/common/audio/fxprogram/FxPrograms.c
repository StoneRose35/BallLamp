#include <stdint.h>
#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram1processSample(int16_t sampleIn,void*data)
{
    int16_t out;
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    pData->updateLock = 1;

    pData->highpass_out = (((((1 << 15) + pData->highpassCutoff) >> 1)*(sampleIn - pData->highpass_old_in))>>15) + ((pData->highpassCutoff *pData->highpass_old_out) >> 15);
    pData->highpass_old_in = sampleIn;
    pData->highpass_old_out = pData->highpass_out;

    out = pData->highpass_out;
    for (uint8_t c=0;c<pData->nWaveshapers;c++)
    {
        out = waveShaperProcessSample(out,&pData->waveshaper1);
    }

    out = out >> 2;
    if (pData->cabSimOnOff == 1)
    {
        out = secondOrderIirFilterProcessSample(out,&pData->filter1);
        out >>= 1;
        out = firFilterProcessSample(out,&pData->filter3);
    }
    pData->updateLock=0;
    return out;
}


void fxProgram1Param1Callback(uint16_t val,void*data) // highpass cutoff before the nonlinear stage
{
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    val <<=  3; 
    if (val > 31500)
    {
        val = 31500;
    }
    else if (val < 10)
    {
        val = 10;
    }
    //while(pData->updateLock > 0);
    pData->highpassCutoff = val;
}

void fxProgram1Param2Callback(uint16_t val,void*data) // number of waveshaper (more means more distortion)
{
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    // map 0-4095 to 1-8
    val >>= 9;
    val += 1; 
    //while(pData->updateLock > 0);
    pData->nWaveshapers = val;
}


void fxProgram1Param3Callback(uint16_t val,void*data) // cab sim on/off
{
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    val >>= 11;
    //while(pData->updateLock > 0);
    pData->cabSimOnOff = val;
}

void fxProgram1Setup(void*data)
{
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    initfirFilter(&pData->filter3);
    initWaveShaper(&pData->waveshaper1,&waveShaperDefaultOverdrive);
}

FxProgram1DataType fxProgram1data = {
    /* butterworth lowpass @ 6000Hz */
    .filter1 = {
        	.coeffB = {3199, 6398, 3199},
            .coeffA = {-30893, 10922},
            .w= {0,0,0}, 
            .bitRes = 16
    },
    .filter3 = {
        .coefficients = {0x62c, 0x674, 0x7d6, 0xbc4, 0x1312, 0x1ea7, 0x2e33, 0x3b3a, 0x3a9e, 0x29bf, 0x15f5, 0x878, 0xf984, 0xebee, 0xe813, 0xe93e, 0xec34, 0xf3d3, 0xfd12, 0x312, 0x5bf, 0x6eb, 0x5da, 0x487, 0x614, 0x771, 0x837, 0x784, 0x299, 0xfc8e, 0xf9f8, 0xfbd5, 0x2b, 0x44c, 0x599, 0x2f3, 0x43, 0x2, 0xfe5b, 0xfc3c, 0xfd4d, 0xb7, 0x4ac, 0x823, 0xa3b, 0xa6a, 0x915, 0x74c, 0x69c, 0x6e5, 0x73e, 0x6cc, 0x4bf, 0x215, 0xffae, 0xfd9b, 0xfde5, 0xffd2, 0x222, 0x3cd, 0x50d, 0x5fa, 0x659, 0x61f}
    },
    .highpass_old_in=0,
    .highpass_old_out=0,
    .highpass_out=0,
    .highpassCutoff = 31000,
    .nWaveshapers = 1,
    .cabSimOnOff = 1
};
FxProgramType fxProgram1 = {
    .name = "Amp-Simulator       ",
    .processSample = &fxProgram1processSample,
    .param1Callback = &fxProgram1Param1Callback,
    .param2Callback = &fxProgram1Param2Callback,
    .param3Callback = &fxProgram1Param3Callback,
    .setup = &fxProgram1Setup,
    .data = (void*)&fxProgram1data
};

FxProgramType* fxPrograms[N_FX_PROGRAMS]={
    &fxProgram1, &fxProgram4, &fxProgram2, &fxProgram3, &fxProgram5, &fxProgram6
};
