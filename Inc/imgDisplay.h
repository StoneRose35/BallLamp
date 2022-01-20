#ifndef _IMG_DISPLAY_H_
#define _IMG_DISPLAY_H_

#include <stdint.h>
#include "system.h"

typedef struct 
{
    uint8_t * colorbytes;
    uint8_t rows;
    uint8_t columns;

} ST7735ImageType;

typedef ST7735ImageType* ST7735Image;

inline uint16_t encodeColor(RGB * clr)
{
    return (((clr->r & 0xF8) | ((clr->g >> 5) & 0x7)) << 8) | 
    (((clr->g & 0x7) << 5) | ((clr->b & 0xF8) >> 3));
}

uint8_t displayImage(ST7735Image img,uint8_t px,uint8_t py);

uint8_t fillSquare(RGB * clr,uint8_t px,uint8_t py,uint8_t sx,uint8_t sy);

#endif
