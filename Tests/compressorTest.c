
#include "stdint.h"
#include "audio/compressor.h"
#include "stdio.h"
#include "stdlib.h"

float int2float(int32_t a)
{
    return (float)a;
}

int32_t float2int(float a)
{
    return (int32_t)a;
}


int main(int argc,char ** argv)
{
    GainFunctionType gainFct;
    int16_t outGain;
    gainFct.gainReduction=2;
    gainFct.threshhold=16384;

    for(int16_t c=0;c<101;c++)
    {
        outGain = applyGain((c-50)*655,abs((c-50)*655),&gainFct);
        printf("Input: %d, Output: %d\n",(c-50)*655,outGain);
    }

}