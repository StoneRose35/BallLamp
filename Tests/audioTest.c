#include <stdint.h>
#include "stdio.h"
#include "audio/sineplayer.h"
#include "audio/secondOrderIirFilter.h"

int main()
{
    FILE * fid;
    int16_t cval;
    int16_t filterIn,filterOut;
    SecondOrderIirFilterType filter1;
    initSecondOrderIirFilter(&filter1);
    filter1.coeffB[0]=3672;
	filter1.coeffB[1]=7343;
	filter1.coeffB[2]=3672;
	filter1.coeffA[0]=-28048;
	filter1.coeffA[1]=9968;
    fid = fopen("audioout.txt","wt");
    //setNote(64);
    cval = -16;
    cval = cval >> 2;
    for (uint16_t c=0;c< 1024;c++)
    {
        if (c==0)
        {
            filterIn = 10000;
        }
        else
        {
            filterIn = 0;
        }
        //cval = getNextSineValue();
        filterOut = secondOrderIirFilterProcessSample(filterIn,&filter1);

        fprintf(fid,"%d\r\n",filterOut);
    }
    fclose(fid);
}