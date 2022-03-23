#include <stdint.h>
#include "stdio.h"
#include "audio/sineplayer.h"
#include "audio/secondOrderIirFilter.h"
#include "audio/waveShaper.h"
#include "math.h"

void impulseTest()
{
    FILE * fid;
    int16_t cval;
    int16_t val_out;
    SecondOrderIirFilterType testFilter = {
        .coeffB = {1007, -2014, 1007},
        .coeffA = {-2013,991},
        .w= {0,0,0},
        .bitRes=11
    };

    fid = fopen("audioout.txt","wt");
    for (uint16_t c=0;c< 1024;c++)
    {
        if (c==0)
        {
            cval = 32767;
        }
        else
        {
            cval = 0;
        }
        val_out = secondOrderIirFilterProcessSample(cval,&testFilter);
        fprintf(fid,"%d\r\n",val_out);
    }
    fclose(fid);
}

int main()
{
    impulseTest();
    /*
    FILE * fid;
    int16_t cval;
    int16_t val_out;
    WaveShaperDataType wavershaper1;

    initWaveShaper(&wavershaper1,&waveShaperDistortion);
    fid = fopen("audioout.txt","wt");
    setNote(64);

    for (uint16_t c=0;c< 1024;c++)
    {
        cval = getNextSineValue();
        val_out = waveShaperProcessSample(cval,&wavershaper1);

        fprintf(fid,"%d\r\n",val_out);
    }
    fclose(fid);
    */
}