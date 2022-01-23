#include "uiStack.h"
#include "apps/uiColors.h"
#include "imgDisplay.h"
#include "charDisplay.h"
#include "images/clock_32x32.h"
#include "images/drafthorse_32x32.h"
#include "images/bulb_on_24x24.h"
#include "images/bulb_off_24x24.h"
#include "datetimeClock.h"
#include "stringFunctions.h"
#include "apps/rootApp.h"
#include "systick.h"
#include <string.h>

#define ROOT_ICON_0_X (80-20-16)
#define ROOT_ICON_1_X (80+20-16)
#define ROOT_ICON_Y (128-32-2)

static struct TriopsBreeder
{
    uint16_t heaterValue; // from 0 to 1023
    uint16_t temperature; //encoded as 4bit fractional fixed point
    uint8_t lampState; // 0 or 1
} triopsController;

static struct RootAppContext
{
    uint8_t entrySelected;
    char timeStr[12];
    uint32_t ticksLast;
} ctx;


void rootAppLoop(void* data)
{
    char tempString[8];
    uint32_t heaterBarWidth;
    if(getTickValue() >ctx.ticksLast+49)
    {
        heaterBarWidth = ((triopsController.heaterValue*(160-16)) >> 10) & 0xFF;
        fillSquare(&heaterClr,8,32,(uint8_t)heaterBarWidth,8);
        fillSquare(&clrBlack,8,40,1,4);
        fillSquare(&clrBlack,8+(160-16)/2,40,1,4);
        fillSquare(&clrBlack,8+(160-16),40,1,4);
        fillSquare(&clrBlack,8+(160-16)/4,40,1,2);
        fillSquare(&clrBlack,8+(160-16)*3/4,40,1,2);

        // display the temperature
        fixedPointUInt16ToChar(tempString,triopsController.temperature,4);
        writeText("T: ",2,6,FONT_TYPE_8X8);
        writeText(tempString,2+2,6,FONT_TYPE_8X8);

        timeToString(ctx.timeStr,getHour(),getMinute(),getSecond());
        // display the time on top
        writeText(ctx.timeStr,16,12,FONT_TYPE_16X16);
        ctx.ticksLast=getTickValue();
    }
}

void rootAppDisplay(void* data)
{
    char tempString[8];
    uint32_t heaterBarWidth;
    // draw background
    fillSquare(&bgclr,0,0,160,128);

    // draw bottom icons
    if (ctx.entrySelected == 2)
    {
        //fillSquare(&bgclr,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
        fillSquare(&entrySel,ROOT_ICON_0_X-2,ROOT_ICON_Y-2,32+4,32+4);
        displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_0_X,ROOT_ICON_Y);
        displayImage(&clock_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
    }
    else if (ctx.entrySelected == 1)
    {
        //fillSquare(&bgclr,ROOT_ICON_0_X-2,ROOT_ICON_Y-2,32+4,32+4);
        fillSquare(&entrySel,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
        displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_0_X,ROOT_ICON_Y);
        displayImage(&clock_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
    }

    // display the time on top
    writeText(ctx.timeStr,16,12,FONT_TYPE_16X16);

    // display the heater level
    // heaterLevel*(width-16)/1024
    // ------------------------------
    // |       |        |        |        |
    // |                |                 |
    heaterBarWidth = ((triopsController.heaterValue*(160-16)) >> 10) & 0xFF;
    fillSquare(&heaterClr,8,32,(uint8_t)heaterBarWidth,8);
    fillSquare(&clrBlack,8,40,1,4);
    fillSquare(&clrBlack,8+(160-16)/2,40,1,4);
    fillSquare(&clrBlack,8+(160-16),40,1,4);
    fillSquare(&clrBlack,8+(160-16)/4,40,1,2);
    fillSquare(&clrBlack,8+(160-16)*3/4,40,1,2);

    // display the temperature
    fixedPointUInt16ToChar(tempString,triopsController.temperature,4);
    writeText("T: ",2,6,FONT_TYPE_8X8);
    writeText(tempString,2+2,6,FONT_TYPE_8X8);

    // display the lamp state
    if(triopsController.lampState == 0)
    {
        displayImage(&bulb_off_24x24_streamimg,40,64);
    }
    else
    {
        displayImage(&bulb_on_24x24_streamimg,40,64);
    }

}

void rootAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    if(encoderIncr > 0 && ctx.entrySelected == 2)
    {
        ctx.entrySelected = 1;
        fillSquare(&bgclr,ROOT_ICON_0_X-2,ROOT_ICON_Y-2,32+4,32+4);
        fillSquare(&entrySel,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
        displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_0_X,ROOT_ICON_Y);
        displayImage(&clock_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
    }
    else if (encoderIncr < 0 && ctx.entrySelected == 1) 
    {
        ctx.entrySelected = 2;
        fillSquare(&bgclr,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
        fillSquare(&entrySel,ROOT_ICON_0_X-2,ROOT_ICON_Y-2,32+4,32+4);
        displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_0_X,ROOT_ICON_Y);
        displayImage(&clock_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
    }
    if (switchChange == -1)
    {
        setPagePtr(ctx.entrySelected);
        display();
    }
} 

void createRootApp(SubApplicationType* app,uint8_t index)
{
    ctx.entrySelected = 2;
    ctx.ticksLast=getTickValue();
    triopsController.heaterValue = 0;
    triopsController.lampState = 0;
    triopsController.temperature = (20 << 4) | (1 << 3); // 20.5 as fixed point
    timeToString(ctx.timeStr,getHour(),getMinute(),getSecond());
    (app+index)->data=NULL;
    (app+index)->display = &rootAppDisplay;
    (app+index)->encoderSwitchCallback = &rootAppEncoderSwitchCallback;
    (app+index)->loop=&rootAppLoop;
}

