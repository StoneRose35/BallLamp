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
    char nrbfr[8];
    if (encoderIncr != 0)
    {
        if(ctx.editLevel == 0) // choose date/time to set or back/ok
        {
            if (encoderIncr > 1 && ctx.encoderPos < 7)
            {   
                setDTDeselectEntity(ctx.encoderPos);
                ctx.encoderPos++;
                setDTSelectEntity(ctx.encoderPos);
            }
            else if (encoderIncr < -1 && ctx.encoderPos > 0)
            {
                setDTDeselectEntity(ctx.encoderPos);
                ctx.encoderPos--;
                setDTSelectEntity(ctx.encoderPos);
            }
        }
        else if (ctx.editLevel==1) // change date/time values
        {
            switch (ctx.encoderPos)
            {
                case SETDT_ENCODERPOS_YEAR:
                    ctx.year += encoderIncr/2;
                    UInt16ToChar(ctx.year,nrbfr);
                    writeText(nrbfr,6*16,8,FONT_TYPE_16X16);
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

                    if(ctx.month < 10)
                    {
                        nrbfr[0]='0';
                        nrbfr[1]=0;
                        writeText(nrbfr,3*16,8,FONT_TYPE_16X16);
                        UInt8ToChar(ctx.month,nrbfr);
                        writeText(nrbfr,4*16,8,FONT_TYPE_16X16);
                    }
                    else
                    {
                        UInt8ToChar(ctx.month,nrbfr);
                        writeText(nrbfr,3*16,8,FONT_TYPE_16X16);
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
                    if(ctx.day < 10)
                    {
                        nrbfr[0]='0';
                        nrbfr[1]=0;
                        writeText(nrbfr,0*16,8,FONT_TYPE_16X16);
                        UInt8ToChar(ctx.day,nrbfr);
                        writeText(nrbfr,1*16,8,FONT_TYPE_16X16);
                    }
                    else
                    {
                        UInt8ToChar(ctx.day,nrbfr);
                        writeText(nrbfr,0*16,8,FONT_TYPE_16X16);
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
                    if(ctx.hour < 10)
                    {
                        nrbfr[0]='0';
                        nrbfr[1]=0;
                        writeText(nrbfr,0*16,32,FONT_TYPE_16X16);
                        UInt8ToChar(ctx.hour,nrbfr);
                        writeText(nrbfr,1*16,32,FONT_TYPE_16X16);
                    }
                    else
                    {
                        UInt8ToChar(ctx.hour,nrbfr);
                        writeText(nrbfr,0*16,32,FONT_TYPE_16X16);
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
                    if(ctx.minute < 10)
                    {
                        nrbfr[0]='0';
                        nrbfr[1]=0;
                        writeText(nrbfr,3*16,32,FONT_TYPE_16X16);
                        UInt8ToChar(ctx.minute,nrbfr);
                        writeText(nrbfr,4*16,32,FONT_TYPE_16X16);
                    }
                    else
                    {
                        UInt8ToChar(ctx.minute,nrbfr);
                        writeText(nrbfr,3*16,32,FONT_TYPE_16X16);
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
                    if(ctx.seconds < 10)
                    {
                        nrbfr[0]='0';
                        nrbfr[1]=0;
                        writeText(nrbfr,6*16,32,FONT_TYPE_16X16);
                        UInt8ToChar(ctx.seconds,nrbfr);
                        writeText(nrbfr,7*16,32,FONT_TYPE_16X16);
                    }
                    else
                    {
                        UInt8ToChar(ctx.seconds,nrbfr);
                        writeText(nrbfr,6*16,32,FONT_TYPE_16X16);
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
                    setDTDeselectEntity(ctx.encoderPos);
                    drawArrows(8*16-8,8,16);
                    ctx.editLevel++;
                    break;
                case SETDT_ENCODERPOS_MONTH:
                    setDTDeselectEntity(ctx.encoderPos);
                    drawArrows(4*16-8,8,16);
                    ctx.editLevel++;
                    break;
                case SETDT_ENCODERPOS_DAY:
                    setDTDeselectEntity(ctx.encoderPos);
                    drawArrows(1*16-8,8,16);
                    ctx.editLevel++;
                    break;            
                case SETDT_ENCODERPOS_HOUR:
                    setDTDeselectEntity(ctx.encoderPos);
                    drawArrows(1*16-8,32,16);
                    ctx.editLevel++;
                    break;  
                case SETDT_ENCODERPOS_MIN:
                    setDTDeselectEntity(ctx.encoderPos);
                    drawArrows(4*16-8,32,16);
                    ctx.editLevel++;
                    break;  
                case SETDT_ENCODERPOS_SEC:
                    setDTDeselectEntity(ctx.encoderPos);
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
                    setDTSelectEntity(ctx.encoderPos);
                    break;
                case SETDT_ENCODERPOS_MONTH:
                    clearArrows(4*16-8,8,16);
                    setDTSelectEntity(ctx.encoderPos);
                    break;
                case SETDT_ENCODERPOS_DAY:
                    clearArrows(1*16-8,8,16);
                    setDTSelectEntity(ctx.encoderPos);
                    break;            
                case SETDT_ENCODERPOS_HOUR:
                    clearArrows(1*16-8,32,16);
                    setDTSelectEntity(ctx.encoderPos);
                    break;  
                case SETDT_ENCODERPOS_MIN:
                    clearArrows(4*16-8,32,16);
                    setDTSelectEntity(ctx.encoderPos);
                    break;  
                case SETDT_ENCODERPOS_SEC:
                    clearArrows(7*16-8,32,16);
                    setDTSelectEntity(ctx.encoderPos);
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

    setDTSelectEntity(ctx.encoderPos);
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


void setDTSelectEntity(uint8_t pos)
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
        drawSelectMarkers(6*16,8,16*4,16);
        break;
    case SETDT_ENCODERPOS_MONTH:
        drawSelectMarkers(16*3,8,16*2,16);
        break;
    case SETDT_ENCODERPOS_DAY:
        drawSelectMarkers(0,8,16*2,16);
        break;
    case SETDT_ENCODERPOS_HOUR:
        drawSelectMarkers(0,32,16*2,16);
        break;
    case SETDT_ENCODERPOS_MIN:
        drawSelectMarkers(3*16,32,16*2,16);
        break;
    case SETDT_ENCODERPOS_SEC:
        drawSelectMarkers(6*16,32,16*2,16);
        break;
    default:
        break;
    }
}

void setDTDeselectEntity(uint8_t pos)
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
        clearSelectMarkers(6*16,8,16*4,16);
        break;
    case SETDT_ENCODERPOS_MONTH:
        clearSelectMarkers(16*3,8,16*2,16);
        break;
    case SETDT_ENCODERPOS_DAY:
        clearSelectMarkers(0,8,16*2,16);
        break;
    case SETDT_ENCODERPOS_HOUR:
        clearSelectMarkers(0,32,16*2,16);
        break;
    case SETDT_ENCODERPOS_MIN:
        clearSelectMarkers(3*16,32,16*2,16);
        break;
    case SETDT_ENCODERPOS_SEC:
        clearSelectMarkers(6*16,32,16*2,16);
        break;
    default:
        break;
    }
}