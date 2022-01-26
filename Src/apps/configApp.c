#include "charDisplay.h"
#include "services/triopsBreederService.h"
#include "stringFunctions.h"
#include "apps/uiColors.h"
#include "imgDisplay.h"
#include "images/OK_32x32.h"
#include "images/back_32x32.h"
#include "uiStack.h"
#include "apps/configApp.h"


static struct configAppData
{
    uint8_t hOff;
    uint8_t minOff;
    uint8_t hOn;
    uint8_t minOn;
    uint32_t mode;
    uint16_t ttarget;
    int16_t heater;
    uint8_t encoderPos;
    uint8_t editLevel;

} ctx;

static const SelectionPositionType configSelectionPositions[5]={
{.posx=10*8,.posy=8,.spacing=8,.width=5*8},
{.posx=10*8,.posy=24,.spacing=8,.width=5*8},
{.posx=10*8,.posy=40,.spacing=8,.width=4*8},
{.posx=10*8,.posy=56,.spacing=8,.width=7*8},
{.posx=10*8,.posy=64,.spacing=8,.width=4*8},
};


void configAppDisplay(void* data)
{
    char nrbfr[8];
    char lineBfr[24];
    char * lineBfrPtr = lineBfr;

    fillSquare(&bgclr,0,0,160,128);

    lineBfrPtr = " Time on  ";
    UInt8ToChar(ctx.hOn,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfrPtr,nrbfr);
    appendToString(lineBfrPtr,":");
    UInt8ToChar(ctx.minOn,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfrPtr,nrbfr);
    appendToString(lineBfrPtr,"     ");
    writeString(lineBfrPtr,0,1);

    lineBfrPtr = " Time off ";
    UInt8ToChar(ctx.hOff,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfrPtr,nrbfr);
    appendToString(lineBfrPtr,":");
    UInt8ToChar(ctx.minOff,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfrPtr,nrbfr);
    appendToString(lineBfrPtr,"     ");
    writeString(lineBfrPtr,0,3);

    lineBfrPtr = " Mode     ";
    if (ctx.mode > 0)
    {
        appendToString(lineBfrPtr,"Auto      ");
    }
    else
    {
        appendToString(lineBfrPtr,"Manual    ");
    }
    writeString(lineBfrPtr,0,5);

    lineBfrPtr = " Ttarget  ";
    fixedPointUInt16ToChar(nrbfr,ctx.ttarget,4);
    appendToString(lineBfrPtr,nrbfr);
    appendToString(lineBfrPtr,"   ");
    writeString(lineBfrPtr,0,7);

    lineBfrPtr = " Heater   ";
    UInt16ToChar(ctx.heater,nrbfr);
    fillWithLeadingZeros(4,nrbfr);
    appendToString(lineBfrPtr,nrbfr);
    appendToString(lineBfrPtr,"      ");
    writeString(lineBfrPtr,0,11);

    displayImage(&OK_32x32_streamimg,160-32-2,128-32-2);
    displayImage(&back_32x32_streamimg,2,128-32-2);

}

void createConfigApp(SubApplicationType* app,uint8_t index)
{
    TriopsControllerType * tdata = getTriopsController();
    ctx.editLevel = 0;
    ctx.encoderPos = 0;
    ctx.hOn = tdata->hourOn;
    ctx.hOff = tdata->hourOff;
    ctx.minOn = tdata->minuteOn;
    ctx.minOff = tdata->minuteOff;
    ctx.mode = tdata->serviceInterval;
    ctx.ttarget = tdata->tTarget;
    ctx.heater = tdata->heaterValue;
    (app+index)->data=(void*)0;
    (app+index)->encoderSwitchCallback=&configAppEncoderSwitchCallback;
    (app+index)->display=&configAppDisplay;
}

void configAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    char nrbfr[8];
    char lineBfr[24];
    char * lineBfrPtr = lineBfr;
    TriopsControllerType * tdata = getTriopsController();
    if(encoderIncr != 0)
    {
        if (ctx.editLevel == 0) // choose entity to edit
        {
            if(encoderIncr > 1 && ctx.encoderPos < 6)
            {
                configDeselectEntity(ctx.encoderPos);
                ctx.encoderPos++;
                configSelectEntity(ctx.encoderPos);
            }
            else if (encoderIncr < -1 && ctx.encoderPos > 0)
            {
                configDeselectEntity(ctx.encoderPos);
                ctx.encoderPos--;
                configSelectEntity(ctx.encoderPos);
            }
        }
        else
        {
            switch (ctx.encoderPos)
            {
                case CONFIG_ENCODERPOS_TIMEON:
                    timedelta(&(ctx.hOff),&(ctx.minOff),encoderIncr);

                    lineBfrPtr = "";
                    UInt8ToChar(ctx.hOn,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfrPtr,nrbfr);
                    appendToString(lineBfrPtr,":");
                    UInt8ToChar(ctx.minOn,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfrPtr,nrbfr);
                    appendToString(lineBfrPtr,"     ");
                    writeString(lineBfrPtr,10,1);
                    break;
                case CONFIG_ENCODERPOS_TIMEOFF:
                    timedelta(&(ctx.hOff),&(ctx.minOff),encoderIncr);

                    lineBfrPtr = "";
                    UInt8ToChar(ctx.hOff,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfrPtr,nrbfr);
                    appendToString(lineBfrPtr,":");
                    UInt8ToChar(ctx.minOff,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfrPtr,nrbfr);
                    appendToString(lineBfrPtr,"     ");
                    writeString(lineBfrPtr,10,3);
                    break;
                case CONFIG_ENCODERPOS_MODE:
                    if (ctx.mode == 0 && encoderIncr > 1)
                    {
                        ctx.mode = 3000;
                    }
                    else if (ctx.mode > 0 && encoderIncr < -1)
                    {
                        ctx.mode = 0;
                    }
                    lineBfrPtr = "";
                    if (ctx.mode > 0)
                    {
                        appendToString(lineBfrPtr,"Auto      ");
                    }
                    else
                    {
                        appendToString(lineBfrPtr,"Manual    ");
                    }
                    writeString(lineBfrPtr,10,5);
                    break;
                case CONFIG_ENCODERPOS_TTARGET:
                    ctx.ttarget += encoderIncr;
                    if (ctx.ttarget <= tdata->tLower)
                    {
                        ctx.ttarget = tdata->tLower + 1;
                    }
                    lineBfrPtr="";
                    fixedPointUInt16ToChar(nrbfr,ctx.ttarget,4);
                    appendToString(lineBfrPtr,nrbfr);
                    appendToString(lineBfrPtr,"   ");
                    writeString(lineBfrPtr,10,7);
                    break;
                case CONFIG_ENCODERPOS_HEATER:
                    ctx.heater += encoderIncr;
                    if (ctx.heater < 0)
                    {
                        ctx.heater = 0;
                    }
                    else if (ctx.heater > 1023)
                    {
                        ctx.heater = 1023;
                    }
                    lineBfrPtr = "";
                    UInt16ToChar(ctx.heater,nrbfr);
                    fillWithLeadingZeros(4,nrbfr);
                    appendToString(lineBfrPtr,nrbfr);
                    appendToString(lineBfrPtr,"      ");
                    writeString(lineBfrPtr,0,11);
                    break;
                default:
                    break;
            }
        }
    }
    if (switchChange == -1)
    {
        if (ctx.editLevel == 0)
        {
            if (ctx.encoderPos < 5)
            {
                configDeselectEntity(ctx.encoderPos);
                drawArrows(configSelectionPositions[ctx.encoderPos].posx + (configSelectionPositions[ctx.encoderPos].width >> 1) - 8,
                           configSelectionPositions[ctx.encoderPos].posy,
                           configSelectionPositions[ctx.encoderPos].spacing);
                ctx.editLevel = 1;
            }
            else if (ctx.encoderPos == CONFIG_ENCODERPOS_OK)
            {
                tdata->serviceInterval = ctx.mode;
                tdata->hourOn = ctx.hOn;
                tdata->minuteOn = ctx.minOn;
                tdata->hourOff = ctx.hOff;
                tdata->minuteOff = ctx.minOff;
                tdata->tTarget = ctx.ttarget;
                tdata->heaterValue = (uint16_t)ctx.heater;
                setPagePtr(0);
                display();
            }
            else if (ctx.encoderPos == CONFIG_ENCODERPOS_BACK)
            {
                setPagePtr(0);
                display();
            }
        }
        else if (ctx.editLevel == 1)
        {
            if (ctx.encoderPos < 5)
            {
                configDeselectEntity(ctx.encoderPos);
                configSelectEntity(ctx.encoderPos);
            }
            ctx.editLevel = 0;
        }
    }
}

void timedelta(uint8_t* hours,uint8_t* min,int16_t delta)
{
    int16_t dHours;
    int16_t dMinutes;
    dHours = delta/60;
    dMinutes = delta - dHours*60;

    *hours += dHours;
    if (*hours > 23)
    {
        *hours = 23;
    }
    else if (*hours < 0)
    {
        *hours = 0;
    }
    *min+=dMinutes;
    if (*min > 59)
    {
        *hours = 59;
    }
    else if (*min < 0)
    {
        *min = 0;
    }
}

void configDeselectEntity(uint8_t pos)
{

    if (pos < 5)
    {
        clearSelectMarkers(configSelectionPositions[pos].posx,configSelectionPositions[pos].posy, configSelectionPositions[pos].width,configSelectionPositions[pos].spacing);
        clearArrows(configSelectionPositions[pos].posx + (configSelectionPositions[pos].width>> 1) - 8,configSelectionPositions[pos].posy,configSelectionPositions[pos].spacing);
    }
    else if (pos == CONFIG_ENCODERPOS_BACK)
    {
        clearSelectFrame(0,128-32-4);
    }
    else if (pos == CONFIG_ENCODERPOS_OK)
    {
        clearSelectFrame(160-32-4,128-32-4);
    }
}

void configSelectEntity(uint8_t pos)
{
    if (pos < 5)
    {
        drawSelectMarkers(configSelectionPositions[pos].posx,configSelectionPositions[pos].posy, configSelectionPositions[pos].width,configSelectionPositions[pos].spacing);
    }
    else if (pos == CONFIG_ENCODERPOS_BACK)
    {
        drawSelectFrame(0,128-32-4);
    }
    else if (pos == CONFIG_ENCODERPOS_OK)
    {
        drawSelectFrame(160-32-4,128-32-4);
    }
}