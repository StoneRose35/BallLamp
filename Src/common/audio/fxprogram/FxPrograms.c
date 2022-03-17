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

    out = out >> 3;
    if (pData->cabSimOnOff == 1)
    {
        out = secondOrderIirFilterProcessSample(out,&pData->filter1);
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
    while(pData->updateLock > 0);
    pData->highpassCutoff = val;
}

void fxProgram1Param2Callback(uint16_t val,void*data) // number of waveshaper (more means more distortion)
{
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    // map 0-4095 to 1-8
    val >>= 9;
    val += 1; 
    while(pData->updateLock > 0);
    pData->nWaveshapers = val;
}


void fxProgram1Param3Callback(uint16_t val,void*data) // cab sim on/off
{
    FxProgram1DataType* pData = (FxProgram1DataType*)data;
    val >>= 11;
    while(pData->updateLock > 0);
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
            .w= {0,0,0} 
    },
    .filter3 = {
        .coefficients = {0xd92, 0xe31, 0x113d, 0x19e0, 0x29f1, 0x436b, 0x659b, 0x8242, 0x80ea, 0x5bcf, 0x304b, 0x12a0, 0xf1bd, 0xd3db, 0xcb60, 0xcdf2, 0xd476, 0xe539, 0xf98f, 0x6c2, 0xca4, 0xf38, 0xcde, 0x9f6, 0xd60, 0x105f, 0x1211, 0x1087, 0x5b7, 0xf86c, 0xf2bb, 0xf6d4, 0x5f, 0x973, 0xc4f, 0x67c, 0x94, 0x5, 0xfc61, 0xf7b8, 0xfa0f, 0x193, 0xa46, 0x11e5, 0x1681, 0x16e9, 0x13f9, 0x100c, 0xe8a, 0xf2a, 0xfef, 0xef4, 0xa70, 0x494, 0xff4a, 0xfabc, 0xfb5e, 0xff9b, 0x4b2, 0x85c, 0xb1c, 0xd25, 0xdf6, 0xd76}
    },
    .highpass_old_in=0,
    .highpass_old_out=0,
    .highpass_out=0,
    .highpassCutoff = 31000,
    .nWaveshapers = 1
};
FxProgramType fxProgram1 = {
    .name = "Amp-Simulator",
    .processSample = &fxProgram1processSample,
    .param1Callback = &fxProgram1Param1Callback,
    .param2Callback = &fxProgram1Param2Callback,
    .param3Callback = &fxProgram1Param3Callback,
    .data = (void*)&fxProgram1data
};

