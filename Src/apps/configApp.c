#include "charDisplay.h"
#include "services/triopsBreederService.h"
#include "stringFunctions.h"
#include "apps/uiColors.h"
#include "imgDisplay.h"
#include "images/OK_32x32.h"
#include "images/back_32x32.h"
#include "images/arrow_left_12x24.h"
#include "images/arrow_right_12x24.h"
#include "uiStack.h"
#include "apps/configApp.h"
#include "heater.h"
#include "remoteSwitch.h"


static struct configAppData
{
    uint8_t hOff;
    uint8_t minOff;
    uint8_t hOn;
    uint8_t minOn;
    uint32_t mode;
    uint16_t ttarget;
    uint16_t tlower;
    uint16_t heater;
    uint16_t cintegral;
    uint16_t serviceInterval;
    uint8_t lamp;
    uint8_t encoderPos;
    uint8_t editLevel;
    uint8_t page;
} ctx;

#define N_SELECTION_POSITIONS 13
#define LAST_PAGE 1

// main configuration for the configuration items, note that the entres for "page back" and "page next"
// must be repeated for each configuration entry, they must be placed at the end of the page items
static const SelectionPositionType configSelectionPositions[N_SELECTION_POSITIONS]={
{.posx=10*8,.posy=1*8,.spacing=8,.width=5*8,.page=0,.editCallback=&hOnChange,.switchCallback=0},
{.posx=10*8,.posy=3*8,.spacing=8,.width=5*8,.page=0,.editCallback=&hOffChange,.switchCallback=0},
{.posx=10*8,.posy=5*8,.spacing=8,.width=4*8,.page=0,.editCallback=&modeChange,.switchCallback=0},
{.posx=10*8,.posy=7*8,.spacing=8,.width=7*8,.page=0,.editCallback=&tTargetChange,.switchCallback=0},
{.posx=10*8,.posy=9*8,.spacing=8,.width=4*8,.page=0,.editCallback=&heaterChange,.switchCallback=0},
{.posx=80-3-12,.posy=128-24-4,.spacing=24,.width=12,.page=0xFE,.editCallback=0,.switchCallback=&pageDecreaseChange}, // back
{.posx=80+3,.posy=128-24-4,.spacing=24,.width=12,.page=0xFF,.editCallback=0,.switchCallback=&pageIncreaseChange}, // next
{.posx=10*8,.posy=1*8,.spacing=8,.width=5*8,.page=1,.editCallback=&lampChange,.switchCallback=0},
{.posx=10*8,.posy=3*8,.spacing=8,.width=5*8,.page=1,.editCallback=&cIntChange,.switchCallback=0},
{.posx=10*8,.posy=5*8,.spacing=8,.width=4*8,.page=1,.editCallback=&tLowerChange,.switchCallback=0},
{.posx=10*8,.posy=7*8,.spacing=8,.width=7*8,.page=1,.editCallback=&serviceIntervalChange,.switchCallback=0},
{.posx=80-3-12,.posy=128-24-4,.spacing=24,.width=12,.page=0xFE,.editCallback=0,.switchCallback=&pageDecreaseChange}, // back
{.posx=80+3,.posy=128-24-4,.spacing=24,.width=12,.page=0xFF,.editCallback=0,.switchCallback=&pageIncreaseChange}, // next
};


void configAppDisplay(void* data)
{

    fillSquare(&bgclr,0,0,160,128); // clear entire screen

    displayCurrentPage();

    displayImage(&arrow_left_12x24_streamimg,80-3-12,128-24-3);
    displayImage(&arrow_right_12x24_streamimg,80+3,128-24-3);
    
    displayImage(&OK_32x32_streamimg,160-32-2,128-32-2);
    displayImage(&back_32x32_streamimg,2,128-32-2);

    configSelectEntity(ctx.encoderPos);
}

void createConfigApp(SubApplicationType* app,uint8_t index)
{
    TriopsControllerType * tdata = getTriopsController();
    ctx.editLevel = 0;
    ctx.encoderPos = 0;
    ctx.page = 0;
    ctx.hOn = tdata->hourOn;
    ctx.hOff = tdata->hourOff;
    ctx.minOn = tdata->minuteOn;
    ctx.minOff = tdata->minuteOff;
    ctx.mode = tdata->serviceInterval;
    ctx.ttarget = tdata->tTarget;
    ctx.tlower = tdata->tLower;
    ctx.heater = tdata->heaterValue;
    ctx.serviceInterval = tdata->serviceInterval;
    ctx.lamp = tdata->lampState;
    ctx.cintegral = tdata->cIntegral;
    (app+index)->data=(void*)0;
    (app+index)->encoderSwitchCallback=&configAppEncoderSwitchCallback;
    (app+index)->display=&configAppDisplay;
}

void configAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    TriopsControllerType * tdata = getTriopsController();
    if(encoderIncr != 0)
    {
        if (ctx.editLevel == 0) // choose entity to edit
        {
            if(encoderIncr > 1 && ctx.encoderPos < N_SELECTION_POSITIONS+1)
            {
                configDeselectEntity(ctx.encoderPos);
                increaseEncoderPos();
                configSelectEntity(ctx.encoderPos);
            }
            else if (encoderIncr < -1 && ctx.encoderPos > 0)
            {
                configDeselectEntity(ctx.encoderPos);
                decreaseEncoderPos();
                configSelectEntity(ctx.encoderPos);
            }
        }
        else // edit entity
        {
            if (configSelectionPositions[ctx.encoderPos].editCallback != 0)
            {
                configSelectionPositions[ctx.encoderPos].editCallback(encoderIncr);
            }
        }
    }
    if (switchChange == -1)
    {
        if (ctx.editLevel == 0)
        {
            if (ctx.encoderPos < N_SELECTION_POSITIONS)
            {
                if (configSelectionPositions[ctx.encoderPos].switchCallback == 0) // pushed on a position without explicit callback, switch to edit level
                {
                    configDeselectEntity(ctx.encoderPos);
                    drawArrows(configSelectionPositions[ctx.encoderPos].posx + (configSelectionPositions[ctx.encoderPos].width >> 1) - 8,
                            configSelectionPositions[ctx.encoderPos].posy,
                            configSelectionPositions[ctx.encoderPos].spacing);
                    ctx.editLevel = 1;
                }
                else
                {
                    configSelectionPositions[ctx.encoderPos].switchCallback();
                }
            }
            else if (ctx.encoderPos == N_SELECTION_POSITIONS + 1) // ok
            {
                tdata->hourOn = ctx.hOn;
                tdata->minuteOn = ctx.minOn;
                tdata->totalMinutesOn = ctx.hOn*60 + ctx.minOn;
                tdata->hourOff = ctx.hOff;
                tdata->minuteOff = ctx.minOff;
                tdata->totalMinutesOff = ctx.hOff*60 + ctx.minOff;
                tdata->tTarget = ctx.ttarget;
                tdata->heaterValue = (uint16_t)ctx.heater;
                tdata->tLower=ctx.tlower;
                tdata->lampState=ctx.lamp;
                tdata->cIntegral=ctx.cintegral;
                if(ctx.mode == CONFIG_MODE_MANUAL)
                {
                    tdata->serviceInterval = 0;
                    tdata->integralTempDeviation = 0;
                }
                else
                {
                    tdata->serviceInterval=ctx.serviceInterval;
                }
                setPagePtr(0);
                display();
            }
            else if (ctx.encoderPos == N_SELECTION_POSITIONS) // back
            {
                setPagePtr(0);
                display();
            }
        }
        else if (ctx.editLevel == 1)
        {
            if (ctx.encoderPos < N_SELECTION_POSITIONS)
            {
                configDeselectEntity(ctx.encoderPos);
                configSelectEntity(ctx.encoderPos);
            }
            ctx.editLevel = 0;
        }
    }
}

void displayCurrentPage()
{
    if (ctx.page == 0)
    {
        displayPage0();
    }
    else if (ctx.page == 1)
    {
        displayPage1();
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

void increaseEncoderPos()
{
    if (ctx.encoderPos < N_SELECTION_POSITIONS-1)
    {
        if (configSelectionPositions[ctx.encoderPos+1].page == ctx.page)
        {
            ctx.encoderPos++;
        }
        else if (configSelectionPositions[ctx.encoderPos+1].page == 0xFE && ctx.page == 0) // would go to page back, skip if on page 0
        {
            ctx.encoderPos+=2;
        }
        else if (configSelectionPositions[ctx.encoderPos+1].page == 0xFE && ctx.page != 0)
        {
            ctx.encoderPos++;
        }
        else if (configSelectionPositions[ctx.encoderPos+1].page == 0xFF && ctx.page == LAST_PAGE) // would go to next page, already at the last, so skip
        {
            ctx.encoderPos+=2;
        }
        else if (configSelectionPositions[ctx.encoderPos+1].page == 0xFF && ctx.page != LAST_PAGE)
        {
            ctx.encoderPos++;
        }
        else // next page would follow --> skip to back/ok
        {
            ctx.encoderPos = N_SELECTION_POSITIONS;
        }
    }
    else // on back, move over to ok
    {
        ctx.encoderPos++;
    }
}

void decreaseEncoderPos()
{
    if (ctx.encoderPos > 0 && ctx.encoderPos < N_SELECTION_POSITIONS)
    {
        if (configSelectionPositions[ctx.encoderPos-1].page == ctx.page)
        {
            ctx.encoderPos--;
        }
        else if (configSelectionPositions[ctx.encoderPos-1].page == 0xFE && ctx.page == 0)
        {
            ctx.encoderPos-=2;
        }
        else if (configSelectionPositions[ctx.encoderPos-1].page == 0xFE && ctx.page != 0)
        {
            ctx.encoderPos--;
        }
        else if (configSelectionPositions[ctx.encoderPos-1].page == 0xFF && ctx.page == LAST_PAGE)
        {
            ctx.encoderPos-=2;
        }
        else if (configSelectionPositions[ctx.encoderPos-1].page == 0xFF && ctx.page != LAST_PAGE)
        {
            ctx.encoderPos--;
        }
    }
    else if (ctx.encoderPos == N_SELECTION_POSITIONS) // on back, jump to the last entry of the page displayed
    {
        uint8_t c;

        for (c=N_SELECTION_POSITIONS - 1;c>=0;c--)
        {
            if(configSelectionPositions[c].page == ctx.page)
            {
                break;
            }
        }
        if (ctx.page==LAST_PAGE)
        {
            ctx.encoderPos = c+1;
        }
        else
        {
            ctx.encoderPos = c+2;
        }
    }
    else
    {
        ctx.encoderPos--;
    }
}

void configDeselectEntity(uint8_t pos)
{

    if (pos < N_SELECTION_POSITIONS)
    {
        clearSelectMarkers(configSelectionPositions[pos].posx,configSelectionPositions[pos].posy, configSelectionPositions[pos].width,configSelectionPositions[pos].spacing);
        if ((configSelectionPositions[pos].page & 0x80)==0) // don't remove arrow on prev/next page
        {
            clearArrows(configSelectionPositions[pos].posx + (configSelectionPositions[pos].width>> 1) - 8,configSelectionPositions[pos].posy,configSelectionPositions[pos].spacing);
        }
    }
    else if (pos == N_SELECTION_POSITIONS) // back
    {
        clearSelectFrame(2,128-32);
    }
    else if (pos == N_SELECTION_POSITIONS+1) // ok
    {
        clearSelectFrame(160-32,128-32);
    }
}

void configSelectEntity(uint8_t pos)
{
    if (pos < N_SELECTION_POSITIONS)
    {
        drawSelectMarkers(configSelectionPositions[pos].posx,configSelectionPositions[pos].posy, configSelectionPositions[pos].width,configSelectionPositions[pos].spacing);
    }
    else if (pos == N_SELECTION_POSITIONS) // back
    {
        drawSelectFrame(2,128-32);
    }
    else if (pos == N_SELECTION_POSITIONS+1) // ok
    {
        drawSelectFrame(160-32,128-32);
    }
}


void hOnChange(int16_t encoderIncr)
{
    char nrbfr[8];
    char lineBfr[24];
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
}

void hOffChange(int16_t encoderIncr)
{
    char nrbfr[8];
    char lineBfr[24];
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
}

void modeChange(int16_t encoderIncr)
{
    char lineBfr[24];
    if (ctx.mode == 0 && encoderIncr > 1)
    {
        ctx.mode = CONFIG_MODE_AUTO;
    }
    else if (ctx.mode > 0 && encoderIncr < -1)
    {
        ctx.mode = CONFIG_MODE_MANUAL;
    }
    lineBfr[0] = 0;
    if (ctx.mode == CONFIG_MODE_AUTO)
    {
        appendToString(lineBfr,"Auto      ");
    }
    else
    {
        appendToString(lineBfr,"Manual    ");
    }
    writeString(lineBfr,10,5);
}

void tTargetChange(int16_t encoderIncr)
{
    char lineBfr[24];
    char nrbfr[8];
    TriopsControllerType * tdata = getTriopsController();
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
}

void heaterChange(int16_t encoderIncr)
{
    char lineBfr[24];
    char nrbfr[8];
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
    if (ctx.mode == CONFIG_MODE_MANUAL)
    {
        setHeater(ctx.heater);
    }
    UInt16ToChar(ctx.heater,nrbfr);
    fillWithLeadingZeros(4,nrbfr);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"      ");
    writeString(lineBfr,10,9);
}


void displayPage0()
{
    char lineBfr[24];
    char nrbfr[8];
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
}


void displayPage1()
{
    char lineBfr[24];
    char nrbfr[8];
    uint16_t cAppended;

    lineBfr[0]=0;
    appendToString(lineBfr," Lamp     ");
    if (ctx.lamp == 1)
    {
        appendToString(lineBfr,"On        ");
    }
    else
    {
        appendToString(lineBfr,"Off       ");
    }
    writeString(lineBfr,0,1);

    lineBfr[0]=0;
    appendToString(lineBfr," cInt     ");
    UInt16ToChar(ctx.cintegral,nrbfr);
    cAppended=appendToString(lineBfr,nrbfr);
    for(uint8_t c=cAppended;c<20;c++)
    {
        appendToString(lineBfr," ");
    }
    writeString(lineBfr,0,3);

    lineBfr[0]=0;
    appendToString(lineBfr," Tlower   ");
    fixedPointUInt16ToChar(nrbfr,ctx.tlower,4);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"   ");
    writeString(lineBfr,0,5);

    lineBfr[0]=0;
    appendToString(lineBfr," Period   ");
    UInt16ToChar(ctx.serviceInterval/100,nrbfr);
    cAppended=appendToString(lineBfr,nrbfr);
    for(uint8_t c=cAppended;c<20;c++)
    {
        appendToString(lineBfr," ");
    }
    writeString(lineBfr,0,7);

    fillSquare(&bgclr,0,9*8,160,8);
}

void lampChange(int16_t encoderIncr)
{
    char lineBfr[24];
    if (encoderIncr > 1)
    {
        ctx.lamp = 1;
    }
    else if (encoderIncr < -1)
    {
        ctx.lamp = 0;
    }
    if(ctx.mode == CONFIG_MODE_MANUAL)
    {
        if (ctx.lamp == 1)
        {
            remoteSwitchOn();
        }
        else
        {
            remoteSwitchOff();
        }
    }
    lineBfr[0]=0;
    if (ctx.lamp == 1)
    {
        appendToString(lineBfr,"On        ");
    }
    else
    {
        appendToString(lineBfr,"Off       ");
    }
    writeString(lineBfr,10,1);
}

void cIntChange(int16_t encoderIncr)
{
    int32_t sval;
    char lineBfr[24];
    char nrbfr[8];
    uint16_t cAppended;
    sval = ctx.cintegral + (encoderIncr/2);
    if (sval < 0)
    {
        ctx.cintegral = 0;
    }
    else
    {
        ctx.cintegral = (uint16_t)sval;
    }

    lineBfr[0]=0;
    UInt16ToChar(ctx.cintegral,nrbfr);
    cAppended = appendToString(lineBfr,nrbfr);
    for(uint8_t c=0;c<10-cAppended;c++)
    {
        appendToString(lineBfr," ");
    }
    writeString(lineBfr,10,3);
}

void tLowerChange(int16_t encoderIncr)
{
    char lineBfr[24];
    char nrbfr[8];
    TriopsControllerType * tdata = getTriopsController();
    ctx.tlower += encoderIncr/2;
    if (ctx.tlower >= tdata->tTarget)
    {
        ctx.tlower = tdata->tTarget - 1;
    }
    lineBfr[0]=0;
    fixedPointUInt16ToChar(nrbfr,ctx.tlower,4);
    appendToString(lineBfr,nrbfr);
    appendToString(lineBfr,"   ");
    writeString(lineBfr,10,5);
}

void serviceIntervalChange(int16_t encoderIncr)
{
    char lineBfr[24];
    char nrbfr[8];
    uint16_t cAppended;
    int32_t sval;
    sval = ctx.serviceInterval + encoderIncr*50;
    if (sval >= 100)
    {
        ctx.serviceInterval = (uint16_t)sval;
    }

    lineBfr[0]=0;
    UInt16ToChar(ctx.serviceInterval/100,nrbfr);
    cAppended = appendToString(lineBfr,nrbfr);
    for(uint8_t c=0;c<10-cAppended;c++)
    {
        appendToString(lineBfr," ");
    }
    writeString(lineBfr,10,7);
}

void pageDecreaseChange()
{
    if(ctx.page > 0)
    {
        ctx.page--;
        for(uint8_t c=0;c<N_SELECTION_POSITIONS;c++)
        {
            if (configSelectionPositions[c].page == ctx.page)
            {
                configDeselectEntity(ctx.encoderPos);
                ctx.encoderPos=c;
                configSelectEntity(ctx.encoderPos);
                break;
            }
        }
        displayCurrentPage();
    }
}

void pageIncreaseChange()
{
    if(ctx.page < LAST_PAGE)
    {
        ctx.page++;
        for(uint8_t c=0;c<N_SELECTION_POSITIONS;c++)
        {
            if (configSelectionPositions[c].page == ctx.page)
            {
                configDeselectEntity(ctx.encoderPos);
                ctx.encoderPos=c;
                configSelectEntity(ctx.encoderPos);
                break;
            }
        }
        displayCurrentPage();
    }
}

void voidCallback(int16_t encoderIncr)
{

}