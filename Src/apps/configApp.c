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
{.posx=10*8,.posy=1*8,.spacing=8,.width=5*8},
{.posx=10*8,.posy=3*8,.spacing=8,.width=5*8},
{.posx=10*8,.posy=5*8,.spacing=8,.width=4*8},
{.posx=10*8,.posy=7*8,.spacing=8,.width=7*8},
{.posx=10*8,.posy=9*8,.spacing=8,.width=4*8},
};


void configAppDisplay(void* data)
{
    char nrbfr[8];
    char lineBfr[24];

    fillSquare(&bgclr,0,0,160,128);

    lineBfr[0]=0;
    appendToString(lineBfr," Time on  ");
    UInt8ToChar(ctx.hOn,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString((char*)lineBfr,nrbfr);
    appendToString(lineBfr,":");
    UInt8ToChar(ctx.minOn,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"     ");
    writeString(lineBfr,0,1);

    lineBfr[0]=0;
    appendToString(lineBfr," Time off ");
    UInt8ToChar(ctx.hOff,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,":");
    UInt8ToChar(ctx.minOff,nrbfr);
    fillWithLeadingZeros(2,nrbfr);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"     ");
    writeString(lineBfr,0,3);

    lineBfr[0]=0;
    appendToString(lineBfr," Mode     ");
    if (ctx.mode > 0)
    {
        appendToString(lineBfr,"Auto      ");
    }
    else
    {
        appendToString(lineBfr,"Manual    ");
    }
    writeString(lineBfr,0,5);

    lineBfr[0]=0;
    appendToString(lineBfr," Ttarget  ");
    fixedPointUInt16ToChar(nrbfr,ctx.ttarget,4);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"   ");
    writeString(lineBfr,0,7);

    lineBfr[0]=0;
    appendToString(lineBfr," Heater   ");
    UInt16ToChar(ctx.heater,nrbfr);
    fillWithLeadingZeros(4,nrbfr);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"      ");
    writeString(lineBfr,0,9);

    displayImage(&OK_32x32_streamimg,160-32-2,128-32-2);
    displayImage(&back_32x32_streamimg,2,128-32-2);

    configSelectEntity(ctx.encoderPos);
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
        else // edit entity
        {
            switch (ctx.encoderPos)
            {
                case CONFIG_ENCODERPOS_TIMEON:
                    timedelta(&(ctx.hOn),&(ctx.minOn),encoderIncr/2);

                    lineBfr[0] = 0;
                    UInt8ToChar(ctx.hOn,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfr,nrbfr);
                    appendToString(lineBfr,":");
                    UInt8ToChar(ctx.minOn,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfr,nrbfr);
                    appendToString(lineBfr,"     ");
                    writeString(lineBfr,10,1);
                    break;
                case CONFIG_ENCODERPOS_TIMEOFF:
                    timedelta(&(ctx.hOff),&(ctx.minOff),encoderIncr/2);

                    lineBfr[0] = 0;
                    UInt8ToChar(ctx.hOff,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfr,nrbfr);
                    appendToString(lineBfr,":");
                    UInt8ToChar(ctx.minOff,nrbfr);
                    fillWithLeadingZeros(2,nrbfr);
                    appendToString(lineBfr,nrbfr);
                    appendToString(lineBfr,"     ");
                    writeString(lineBfr,10,3);
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
                    lineBfr[0] = 0;
                    if (ctx.mode > 0)
                    {
                        appendToString(lineBfr,"Auto      ");
                    }
                    else
                    {
                        appendToString(lineBfr,"Manual    ");
                    }
                    writeString(lineBfr,10,5);
                    break;
                case CONFIG_ENCODERPOS_TTARGET:
                    ctx.ttarget += encoderIncr/2;
                    if (ctx.ttarget <= tdata->tLower)
                    {
                        ctx.ttarget = tdata->tLower + 1;
                    }
                    lineBfr[0]=0;
                    fixedPointUInt16ToChar(nrbfr,ctx.ttarget,4);
                    appendToString(lineBfr,nrbfr);
                    appendToString(lineBfr,"   ");
                    writeString(lineBfr,10,7);
                    break;
                case CONFIG_ENCODERPOS_HEATER:
                    ctx.heater += encoderIncr/2;
                    if (ctx.heater < 0)
                    {
                        ctx.heater = 0;
                    }
                    else if (ctx.heater > 1023)
                    {
                        ctx.heater = 1023;
                    }
                    lineBfr[0] = 0;
                    UInt16ToChar(ctx.heater,nrbfr);
                    fillWithLeadingZeros(4,nrbfr);
                    appendToString(lineBfr,nrbfr);
                    appendToString(lineBfr,"      ");
                    writeString(lineBfr,10,9);
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
                tdata->totalMinutesOn = ctx.hOn*60 + ctx.minOn;
                tdata->hourOff = ctx.hOff;
                tdata->minuteOff = ctx.minOff;
                tdata->totalMinutesOff = ctx.hOff*60 + ctx.minOff;
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
    int16_t sMin = *min;
    int16_t sHours = *hours;
    dHours = delta/60;
    dMinutes = delta - dHours*60;

    sMin+=dMinutes;
    if (sMin > 59)
    {
        dHours++;
        sMin -= 60;
    }
    else if (sMin < 0)
    {
        dHours--;
        sMin = 60 + sMin;
    }
    *min=(uint8_t)sMin;

    sHours += dHours;
    if (sHours > 23)
    {
        sHours = 23;
        *min=59;
    }
    else if (sHours < 0)
    {
        sHours = 0;
        *min=0;
    }
    *hours=(uint8_t)sHours;
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
        clearSelectFrame(2,128-32);
    }
    else if (pos == CONFIG_ENCODERPOS_OK)
    {
        clearSelectFrame(160-32,128-32);
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
        drawSelectFrame(2,128-32);
    }
    else if (pos == CONFIG_ENCODERPOS_OK)
    {
        drawSelectFrame(160-32,128-32);
    }
}