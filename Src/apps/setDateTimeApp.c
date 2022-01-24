#include "stdint.h"
#include "apps/uiColors.h"
#include "uiStack.h"
#include "apps/setDateTimeApp.h"
#include "imgDisplay.h"
#include "charDisplay.h"
#include "stringFunctions.h"
#include "datetimeClock.h"
#include "images/OK_32x32.h"
#include "images/back_32x32.h"
#include "images/arrow_up_16x8.h"
#include "images/arrow_down_16x8.h"

static struct setDateTimeAppData
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;
    uint8_t encoderPos;
    uint8_t editLevel;

} ctx;

void setDateTimeAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    int8_t nval;
    uint8_t maxDayFeb;
    if (encoderIncr != 0)
    {
        if(ctx.editLevel == 0) // choose date/time to set or back/ok
        {
            if (encoderIncr > 1 && ctx.encoderPos < 7)
            {   
                deselectEntity(ctx.encoderPos);
                ctx.encoderPos++;
                selectEntity(ctx.encoderPos);
            }
            else if (encoderIncr < -1 && ctx.encoderPos > 0)
            {
                deselectEntity(ctx.encoderPos);
                ctx.encoderPos--;
                selectEntity(ctx.encoderPos);
            }
        }
        else if (ctx.editLevel==1) // change date/time values
        {
            switch (ctx.encoderPos)
            {
                case SETDT_ENCODERPOS_YEAR:
                    ctx.year += encoderIncr/2;
                    break;
                case SETDT_ENCODERPOS_MONTH:
                    nval = ctx.month + encoderIncr/2;
                    if (nval < 1)
                    {
                        ctx.month=1;
                    }
                    else if(nval > 12)
                    {
                        ctx.month=12;
                    }
                    else
                    {
                        ctx.month=nval;
                    }
                    break;
                case SETDT_ENCODERPOS_DAY:
                    nval = ctx.day + encoderIncr/2;
                    if (ctx.month==1 || ctx.month==3 || ctx.month==5 || ctx.month==7 || ctx.month==8 || ctx.month==10 || ctx.month==12)
                    {
                        if (nval < 1)
                        {
                            ctx.day=1;
                        }
                        else if(nval > 31)
                        {
                            ctx.day=31;
                        }
                        else
                        {
                            ctx.day=nval;
                        }
                    }
                    else if (ctx.month==4 || ctx.month==6 || ctx.month==9 || ctx.month==11)
                    {
                        if (nval < 1)
                        {
                            ctx.day=1;
                        }
                        else if(nval > 30)
                        {
                            ctx.day=30;
                        }
                        else
                        {
                            ctx.day=nval;
                        }
                    }
                    else // february
                    {
                        maxDayFeb = ((ctx.year & 0xFFFC) > 0) ? 28 : 29; // 29 if leap year: year dividable be 4, the case when the first two bits are 0
                        if (nval < 1)
                        {
                            ctx.day=1;
                        }
                        else if(nval > maxDayFeb)
                        {
                            ctx.day=maxDayFeb;
                        }
                        else
                        {
                            ctx.day=nval;
                        }
                    }
                    break;
                case SETDT_ENCODERPOS_HOUR:
                    nval = ctx.hour + encoderIncr/2;
                    if (nval < 0)
                    {
                        ctx.hour=0;
                    }
                    else if(nval > 23)
                    {
                        ctx.hour = 23;
                    }
                    else
                    {
                        ctx.hour=nval;
                    }
                    break;
                case SETDT_ENCODERPOS_MIN:
                    nval = ctx.minute + encoderIncr/2;
                    if (nval < 0)
                    {
                        ctx.minute=0;
                    }
                    else if(nval > 59)
                    {
                        ctx.minute = 59;
                    }
                    else
                    {
                        ctx.minute=nval;
                    }
                    break;
                case SETDT_ENCODERPOS_SEC:
                    nval = ctx.seconds + encoderIncr/2;
                    if (nval < 0)
                    {
                        ctx.seconds=0;
                    }
                    else if(nval > 59)
                    {
                        ctx.seconds = 59;
                    }
                    else
                    {
                        ctx.seconds=nval;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    if (switchChange == -1)
    {
        if (ctx.editLevel==0) // switch to edit level 1 (change numbers) if on a number, go back with or without saving the date and time
        {
            switch (ctx.encoderPos)
            {
                case SETDT_ENCODERPOS_YEAR:
                    deselectEntity(ctx.encoderPos);
                    drawArrows(8*16-8,8,16);
                    ctx.editLevel++;
                    break;
                case SETDT_ENCODERPOS_MONTH:
                    deselectEntity(ctx.encoderPos);
                    drawArrows(4*16-8,8,16);
                    ctx.editLevel++;
                    break;
                case SETDT_ENCODERPOS_DAY:
                    deselectEntity(ctx.encoderPos);
                    drawArrows(1*16-8,8,16);
                    ctx.editLevel++;
                    break;            
                case SETDT_ENCODERPOS_HOUR:
                    deselectEntity(ctx.encoderPos);
                    drawArrows(1*16-8,32,16);
                    ctx.editLevel++;
                    break;  
                case SETDT_ENCODERPOS_MIN:
                    deselectEntity(ctx.encoderPos);
                    drawArrows(4*16-8,32,16);
                    ctx.editLevel++;
                    break;  
                case SETDT_ENCODERPOS_SEC:
                    deselectEntity(ctx.encoderPos);
                    drawArrows(7*16-8,32,16);
                    ctx.editLevel++;
                    break;  
                case SETDT_ENCODERPOS_OK: // explicitely fall through (no break needed)
                    setYear(ctx.year);
                    setMonth(ctx.month);
                    setDay(ctx.day);
                    setHour(ctx.hour);
                    setMinute(ctx.minute);
                    setSecond(ctx.seconds);
                case SETDT_ENCODERPOS_BACK:
                    setPagePtr(0);
                    display();
                    break;
                default:
                    break;
            }
        }
        else if (ctx.editLevel == 1)
        {
            switch (ctx.encoderPos)
            {
                case SETDT_ENCODERPOS_YEAR:
                    clearArrows(8*16-8,8,16);
                    selectEntity(ctx.encoderPos);
                    break;
                case SETDT_ENCODERPOS_MONTH:
                    clearArrows(4*16-8,8,16);
                    selectEntity(ctx.encoderPos);
                    break;
                case SETDT_ENCODERPOS_DAY:
                    clearArrows(1*16-8,8,16);
                    selectEntity(ctx.encoderPos);
                    break;            
                case SETDT_ENCODERPOS_HOUR:
                    clearArrows(1*16-8,32,16);
                    selectEntity(ctx.encoderPos);
                    break;  
                case SETDT_ENCODERPOS_MIN:
                    clearArrows(4*16-8,32,16);
                    selectEntity(ctx.encoderPos);
                    break;  
                case SETDT_ENCODERPOS_SEC:
                    clearArrows(7*16-8,32,16);
                    selectEntity(ctx.encoderPos);
                    break;  
                default:
                    break;
            }
            ctx.editLevel--;
        }
    }
}

void setDateTimeAppDisplay(void* data)
{
    char dtbfr[24];
    dateToString(dtbfr,ctx.year,ctx.month,ctx.day);
    fillSquare(&bgclr,0,0,160,128);
    writeText(dtbfr,0,8,FONT_TYPE_16X16);
    timeToString(dtbfr,ctx.hour,ctx.minute,ctx.seconds);
    writeText(dtbfr,0,32,FONT_TYPE_16X16);

    displayImage(&OK_32x32_streamimg,160-32-2,128-32-2);
    displayImage(&back_32x32_streamimg,2,128-32-2);

    selectEntity(ctx.encoderPos);
}

void createSetDateTimeApp(SubApplicationType* app,uint8_t index)
{
    ctx.year=getYear();
    ctx.month=getMonth();
    ctx.day =getDay();
    ctx.hour = getHour();
    ctx.minute = getMinute();
    ctx.seconds = getSecond();
    ctx.encoderPos=SETDT_ENCODERPOS_HOUR;
    ctx.editLevel=0;
    (app+index)->data=(void*)0;
    (app+index)->encoderSwitchCallback=&setDateTimeAppEncoderSwitchCallback;
    (app+index)->display=&setDateTimeAppDisplay;
    (app+index)->loop=(void*)0;
}


void selectEntity(uint8_t pos)
{
    switch (pos)
    {
    case SETDT_ENCODERPOS_BACK:
        drawSelectFrame(2,128-32);
        break;
    case SETDT_ENCODERPOS_OK:
        drawSelectFrame(160-32,128-32);
        break;
    case SETDT_ENCODERPOS_YEAR:
        drawSelectMarkers(6*16,8,16*4);
        break;
    case SETDT_ENCODERPOS_MONTH:
        drawSelectMarkers(16*3,8,16*2);
        break;
    case SETDT_ENCODERPOS_DAY:
        drawSelectMarkers(0,8,16*2);
        break;
    case SETDT_ENCODERPOS_HOUR:
        drawSelectMarkers(0,32,16*2);
        break;
    case SETDT_ENCODERPOS_MIN:
        drawSelectMarkers(3*16,32,16*2);
        break;
    case SETDT_ENCODERPOS_SEC:
        drawSelectMarkers(6*16,32,16*2);
        break;
    default:
        break;
    }
}

void deselectEntity(uint8_t pos)
{
    switch (pos)
    {
    case SETDT_ENCODERPOS_BACK:
        clearSelectFrame(2,128-32);
        break;
    case SETDT_ENCODERPOS_OK:
        clearSelectFrame(160-32,128-32);
        break;
    case SETDT_ENCODERPOS_YEAR:
        clearSelectMarkers(6*16,8,16*4);
        break;
    case SETDT_ENCODERPOS_MONTH:
        clearSelectMarkers(16*3,8,16*2);
        break;
    case SETDT_ENCODERPOS_DAY:
        clearSelectMarkers(0,8,16*2);
        break;
    case SETDT_ENCODERPOS_HOUR:
        clearSelectMarkers(0,32,16*2);
        break;
    case SETDT_ENCODERPOS_MIN:
        clearSelectMarkers(3*16,32,16*2);
        break;
    case SETDT_ENCODERPOS_SEC:
        clearSelectMarkers(6*16,32,16*2);
        break;
    default:
        break;
    }
}

/**
 * @brief draws at 2 pixel tall bar spaced 16 pixel vertically apart
 * 
 * @param posx position in pixel
 * @param posy position in pixels
 * @param width width in pixels
 */
void drawSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width)
{
    fillSquare(&entrySel,posx,posy-2,width,2);
    fillSquare(&entrySel,posx,posy+16,width,2);
}


void clearSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width)
{
    fillSquare(&bgclr,posx,posy-2,width,2);
    fillSquare(&bgclr,posx,posy+16,width,2);
}

void drawSelectFrame(uint8_t posx,uint8_t posy)
{
    fillSquare(&entrySel,posx-2,posy-2,36,2); // top
    fillSquare(&entrySel,posx-2,posy-2,2,36); // left
    fillSquare(&entrySel,posx+32,posy-2,2,36); // right
    fillSquare(&entrySel,posx-2,posy+32-2,36,2); // bottom
}

void clearSelectFrame(uint8_t posx,uint8_t posy)
{
    fillSquare(&bgclr,posx-2,posy-2,36,2); // top
    fillSquare(&bgclr,posx-2,posy-2,2,36); // left
    fillSquare(&bgclr,posx+32,posy-2,2,36); // right
    fillSquare(&bgclr,posx-2,posy+32-2,36,2); // bottom
}

void drawArrows(uint8_t px, uint8_t py,uint8_t spacing)
{
    displayImage(&arrow_up_16x8_streamimg,px,py-8);
    displayImage(&arrow_down_16x8_streamimg,px,py+spacing);
}

void clearArrows(uint8_t px, uint8_t py,uint8_t spacing)
{
    fillSquare(&bgclr,px,py-8,16,8);
    fillSquare(&bgclr,px,py+spacing,16,8);
}
