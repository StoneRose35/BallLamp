#include "imgDisplay.h"
#include "charDisplay.h"
#include "stdlib.h"
#include "spi_sdcard_display.h"

uint8_t displayImage(ST7735Image img,uint8_t px,uint8_t py)
{
    if ((px + img->columns <160) && (py + img->rows < 128))
    {
        casetCmd(px,px+img->columns);
        rasetCmd(py,py+img->rows);
        sendDisplayCommand(0x2C,img->colorbytes,img->rows*img->columns*2);
        return 0;
    }
    else
    {
        return 1;
    }
}


uint8_t fillSquare(RGB * clr,uint8_t px,uint8_t py,uint8_t sx,uint8_t sy)
{
    if (sx > 0 && sy > 0)
    {
        uint16_t * squareBfr = (uint16_t*)malloc(sx*sy*2);
        uint16_t clrEncoded = encodeColor(clr);
        for (uint32_t c=0;c<sx*sy;c++)
        {
            *(squareBfr + c) = clrEncoded;
        }
        casetCmd(px,px+sx);
        rasetCmd(py,py+sy);
        sendDisplayCommand(0x2C,(uint8_t*)squareBfr,sx*sy*2);
        free(squareBfr);
    }
    return 0;
}

