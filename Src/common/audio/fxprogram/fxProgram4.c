#include <stdint.h>
#include "audio/fxprogram/fxProgram.h"

int16_t fxProgram4processSample(int16_t sampleIn,void*data)
{
    int16_t out;
    FxProgram4DataType* pData = (FxProgram4DataType*)data;
    pData->updateLock = 1;

    pData->highpass_out = (((((1 << 15) + 31000) >> 1)*(sampleIn - pData->highpass_old_in))>>15) + ((31000 *pData->highpass_old_out) >> 15);
    pData->highpass_old_in = sampleIn;
    pData->highpass_old_out = pData->highpass_out;

    out = pData->highpass_out;
    //out = gainStageProcessSample(out, &pData->gainStage);

    out = waveShaperProcessSample(out,&pData->waveshaper1.waveshaper); //  OversamplingDistortionProcessSample(out,&pData->waveshaper1);

    out = out >> 2;
    if (pData->cabSimOnOff == 1)
    {
        //out = secondOrderIirFilterProcessSample(out,&pData->filter1);
        out = secondOrderIirFilterProcessSample(out,&pData->filter2);
        //out = secondOrderIirFilterProcessSample(out,&pData->filter3);
    }
    pData->updateLock=0;
    return out;
}


void fxProgram4Param1Callback(uint16_t val,void*data) // gain
{
    FxProgram4DataType* pData = (FxProgram4DataType*)data;

    pData->gainStage.gain=val<<2;
}

void fxProgram4Param2Callback(uint16_t val,void*data) // number of waveshaper (more means more distortion)
{
    //FxProgram4DataType* pData = (FxProgram4DataType*)data;

    //pData->nWaveshapers = val;
}


void fxProgram4Param3Callback(uint16_t val,void*data) // cab sim on/off
{
    FxProgram4DataType* pData = (FxProgram4DataType*)data;
    val >>= 11;
    //while(pData->updateLock > 0);
    pData->cabSimOnOff = val;
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
            .w= {0,0,0} 
    },
    /*butterworth highpass @170Hz*/
    .filter2 = {
        .coeffB = {32255, -64510, 32255},
        .coeffA = {-64502, 31751},
        .w= {0,0,0} 
    },
    /*Chebychev type 1 notch filter from 100 to 700 Hz*/
    .filter3 = {
        .coeffB = {29484, -55659, 27846},
        .coeffA = {-55659, 24564},
        .w= {0,0,0} 
    },
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