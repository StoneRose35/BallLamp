#include <stdint.h>
#include "audio/fxprogram/fxProgram.h"
#include "stringFunctions.h"

#define P7_HIGHPASS 30000


static int16_t fxProgram7processSample(int16_t sampleIn,void*data)
{
    int32_t out;
    FxProgram7DataType* pData = (FxProgram7DataType*)data;

    pData->highpass_out = (((((1 << 15) + P7_HIGHPASS) >> 1)*(sampleIn - pData->highpass_old_in))>>15) + ((P7_HIGHPASS *pData->highpass_old_out) >> 15);
    pData->highpass_old_in = sampleIn;
    pData->highpass_old_out = pData->highpass_out;

    out = pData->highpass_out;
    //out = sampleIn;
    out = gainStageProcessSample(out, &pData->gainStage);


    out = waveShaperProcessSample(out,&pData->waveshaper1);
    out = compressorProcessSample(out,&pData->compressor);
    out = waveShaperProcessSample(out,&pData->waveshaper2);
    out = waveShaperProcessSample(out,&pData->waveshaper3);


    out = out >> 1;
    if (pData->cabSimType == 0)
    {
        out >>= 2;
        out = firFilterProcessSample(out,&pData->hiwattFir);
    }
    else if (pData->cabSimType == 1)
    {
        out >>= 2;
        out = firFilterProcessSample(out,&pData->frontmanFir);
    }
    else if (pData->cabSimType == 2)
    {
        out >>= 2;
        out = firFilterProcessSample(out,&pData->voxAC15Fir);
    }

    out = delayLineProcessSample(out, pData->delay);

    return out;
}

static void fxProgramParam1Callback(uint16_t val,void*data) // gain
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    pData->gainStage.gain = ((val << 3) + 0x3F); 
}

static void fxProgramParam1Display(void*data,char*res)
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    fixedPointInt16ToChar(res,pData->gainStage.gain,8);
}


static void fxProgramParam2Callback(uint16_t val,void*data) // Compressor Threshhold
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    pData->compressor.gainFunction.threshhold = (val << 3);
}

static void fxProgramParam2Display(void*data,char*res)
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    Int16ToChar(pData->compressor.gainFunction.threshhold,res);
}


static void fxProgramParam3Callback(uint16_t val,void*data) // delay intensity
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    pData->delay->delayInSamples = 2400 + (val << 3);
    pData->delay->mix = val << 2; // up to 100%
    pData->delay->feedback = (1<< 14);
}

static void fxProgramParam3Display(void*data,char*res)
{
    int16_t dVal;
    FxProgramType* fData = (FxProgramType*)data;
    dVal=fData->parameters[2].rawValue/328;
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

static void fxProgramParam4Callback(uint16_t val,void*data) // cab type
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    pData->cabSimType = (val >> 10);
}

static void fxProgramParam4Display(void*data,char*res)
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    for(uint8_t c=0;c<24;c++)
    {
        *(res+c) =  pData->cabNames[pData->cabSimType][c];
    }
}



static void fxProgram7Setup(void*data)
{
    FxProgram7DataType* pData = (FxProgram7DataType*)data;
    initWaveShaper(&pData->waveshaper1,&waveShaperDefaultOverdrive);
    initWaveShaper(&pData->waveshaper2,&waveShaperSoftOverdrive);
    initWaveShaper(&pData->waveshaper3,&waveShaperCurvedOverdrive);
    initDelay(pData->delay);
    initfirFilter(&pData->frontmanFir);
    initfirFilter(&pData->hiwattFir);
    initfirFilter(&pData->voxAC15Fir);
    setAttack(300,&pData->compressor);
    setRelease(3000,&pData->compressor);
    initDelay(pData->delay);
}

FxProgram7DataType fxProgram7data = {
    .hiwattFir = {
        .coefficients= {-44, -44, -7, 329, 1426, 3715, 7285, 11592, 15039, 15011, 9474, 4, -7820, -9601, -8422, -6903, -4139, -599, 2363, 3189, 1808, 220, 269, 846, 683, 349, -45, -494, -493, -330, -548, -810, -402, 619, 1288, 785, -661, -1696, -2115, -1968, -1125, -222, 383, 447, 33, -300, -135, 138, 152, 153, 148, -73, -327, -518, -599, -500, -329, -204, -95, 22, 53, 8, -11, 0}
        },
    .frontmanFir = {
        .coefficients = {-683, -2337, -4441, -7066, -10361, -13202, -13141, -8988, -2892, 1373, 2350, 1469, 25, 280, 2913, 5652, 5864, 3615, 173, -3115, -4922, -3896, 275, 5430, 8479, 7426, 3956, 1727, 957, -250, -2484, -4381, -4417, -2557, -47, 1879, 2384, 2073, 1460, -340, -2704, -3122, -1003, 1695, 3031, 2931, 2448, 2146, 2183, 2400, 1684, 389, -538, -694, -397, -137, -163, -264, -103, 281, 527, 380, 83, 0}
    },
    .voxAC15Fir = {
        .coefficients = {-1165, -3281, -6190, -8822, -10681, -11434, -12841, -14247, -10546, -92, 8163, 6631, 491, -3037, -2164, 37, 2226, 3009, 2363, 1741, 2194, 2227, -174, -1760, 2136, 4816, 2360, 1174, 443, -687, -269, 1026, 1683, 1932, 2341, 2203, 1018, 137, 224, 735, 1116, 1254, 1017, 339, -396, -597, -251, 45, 442, 960, 36, -409, -21, 141, 174, 261, 218, 106, 131, 138, 25, 23, 27, 0}
    },
    .cabNames = {
        "Hiwatt M412        ", 
        "Fender Frontman    ",
        "Vox AC15           ",
        "Off                "
    },
    .gainStage.gain=512,
    .gainStage.offset=0,
    .cabSimType = 1,
    .compressor.gainFunction.gainReduction=2,
    .compressor.currentAvg=0,
    .compressor.gainFunction.threshhold = 20000
};

FxProgramType fxProgram7 = {
    .name = "Amp Model 3          ",
    .nParameters=4,
    .parameters = {
        {
            .name = "Gain           ",
            .control=0,
            .increment=64,
            .rawValue=0,
            .getParameterDisplay=&fxProgramParam1Display,
            .getParameterValue=0,
            .setParameter=&fxProgramParam1Callback
        },
        {
            .name = "Comp Thrh      ",
            .control=1,
            .increment=64,
            .rawValue=0,
            .getParameterDisplay=&fxProgramParam2Display,
            .getParameterValue=0,
            .setParameter=&fxProgramParam2Callback
        },   
        {
            .name = "Delay Int      ",
            .control=2,
            .increment=64,
            .rawValue=0,
            .getParameterDisplay=&fxProgramParam3Display,
            .getParameterValue=0,
            .setParameter=&fxProgramParam3Callback
        },
        {
            .name = "Cab Type       ",
            .control=3,
            .increment=1024,
            .rawValue=0,
            .getParameterDisplay=&fxProgramParam4Display,
            .getParameterValue=0,
            .setParameter=&fxProgramParam4Callback
        }   
    },
    .processSample = &fxProgram7processSample,
    .setup = &fxProgram7Setup,
    .data = (void*)&fxProgram7data
};