#include <stdint.h>
#include "stdio.h"
#include "audio/sineplayer.h"

int main()
{
    FILE * fid;
    int16_t cval;
    fid = fopen("audioout.txt","wt");
    setNote(64);
    cval = -16;
    cval = cval >> 2;
    for (uint16_t c=0;c< 1024;c++)
    {
        cval = getNextSineValue();
        fprintf(fid,"%d\r\n",cval);
    }
    fclose(fid);
}