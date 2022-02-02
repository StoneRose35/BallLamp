#include "uiStack.h"
#include "apps/uiColors.h"
#include "imgDisplay.h"
#include "charDisplay.h"
#include "images/clock_32x32.h"
#include "images/drafthorse_32x32.h"
#include "images/gearwheel_32x32.h"
#include "images/bulb_on_24x24.h"
#include "images/bulb_off_24x24.h"
#include "images/warning_sd_card_16x16.h"
#include "images/warning_tempsensor_16x16.h"
#include "datetimeClock.h"
#include "stringFunctions.h"
#include "apps/rootApp.h"
#include "systick.h"
#include <string.h>
#include "services/triopsBreederService.h"

#define ROOT_ICON_1_X (20-16)
#define ROOT_ICON_2_X (60-16)
#define ROOT_ICON_3_X (100-16)
#define ROOT_ICON_Y (128-32-2)



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
    TriopsControllerType* triopsController = getTriopsController();
    if(getTickValue() >ctx.ticksLast+99)
    {
        //fillSquare(&bgclr,8,32,160-8,8);
        heaterBarWidth = ((triopsController->heaterValue*(160-16)) >> 10) & 0xFF;
        fillSquare(&heaterClr,8,32,(uint8_t)heaterBarWidth,8);
        fillSquare(&bgclr,(uint8_t)heaterBarWidth+1+8,32,160-8-(uint8_t)heaterBarWidth,8);

        // display the temperature
        fixedPointUInt16ToChar(tempString,triopsController->temperature,4);
        writeText("T: ",2,6,FONT_TYPE_8X8);
        writeText(tempString,2+2,6,FONT_TYPE_8X8);

        timeToString(ctx.timeStr,getHour(),getMinute(),getSecond());
        // display the time on top
        writeText(ctx.timeStr,16,12,FONT_TYPE_16X16);
        ctx.ticksLast=getTickValue();

        // display the lamp state
        if(triopsController->lampState == 0)
        {
            displayImage(&bulb_off_24x24_streamimg,8,64);
        }
        else
        {
            displayImage(&bulb_on_24x24_streamimg,8,64);
        }

        // display the warnings
        if ((triopsController->errorFlags & TC_ERROR_FILESYSTEM) != 0)
        {
            displayImage(&warning_sd_card_16x16_streamimg,160-8-16,64);
        }
        else
        {
            fillSquare(&bgclr,160-8-16,64,16,16);
        }

        if((triopsController->errorFlags & TC_ERROR_THERMOMETER) != 0)
        {
            displayImage(&warning_tempsensor_16x16_streamimg,160-8-16-4-16,64);
        }
        else
        {
            fillSquare(&bgclr,160-8-16-4-16,64,16,16);
        }
    }
}

void rootAppDisplay(void* data)
{
    char tempString[8];
    uint32_t heaterBarWidth;
    // draw background
    fillSquare(&bgclr,0,0,160,128);
    TriopsControllerType* triopsController = getTriopsController();
    // draw bottom icons
    switch (ctx.entrySelected)
    {
        case 3:
            fillSquare(&entrySel,ROOT_ICON_3_X-2,ROOT_ICON_Y-2,32+4,32+4);
            break;
        case 2:
            fillSquare(&entrySel,ROOT_ICON_2_X-2,ROOT_ICON_Y-2,32+4,32+4);
            break;
        case 1:
            fillSquare(&entrySel,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
            break;
    }
    displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
    displayImage(&clock_32x32_streamimg,ROOT_ICON_2_X,ROOT_ICON_Y);
    displayImage(&gearwheel_32x32_streamimg,ROOT_ICON_3_X,ROOT_ICON_Y);
    
    // display the time on top
    writeText(ctx.timeStr,16,12,FONT_TYPE_16X16);

    // display the heater level
    // heaterLevel*(width-16)/1024
    // ------------------------------
    // |       |        |        |        |
    // |                |                 |
    heaterBarWidth = ((triopsController->heaterValue*(160-16)) >> 10) & 0xFF;
    fillSquare(&heaterClr,8,32,(uint8_t)heaterBarWidth,8);
    fillSquare(&clrBlack,8,40,1,4);
    fillSquare(&clrBlack,8+(160-16)/2,40,1,4);
    fillSquare(&clrBlack,8+(160-16),40,1,4);
    fillSquare(&clrBlack,8+(160-16)/4,40,1,2);
    fillSquare(&clrBlack,8+(160-16)*3/4,40,1,2);

    // display the temperature
    fixedPointUInt16ToChar(tempString,triopsController->temperature,4);
    writeText("T: ",2,6,FONT_TYPE_8X8);
    writeText(tempString,2+2,6,FONT_TYPE_8X8);

    // display the lamp state
    if(triopsController->lampState == 0)
    {
        displayImage(&bulb_off_24x24_streamimg,8,64);
    }
    else
    {
        displayImage(&bulb_on_24x24_streamimg,8,64);
    }

    // display the warnings
    if ((triopsController->errorFlags & TC_ERROR_FILESYSTEM) != 0)
    {
        displayImage(&warning_sd_card_16x16_streamimg,160-8-16,64);
    }
    else
    {
        fillSquare(&bgclr,160-8-16,64,16,16);
    }

    if((triopsController->errorFlags & TC_ERROR_THERMOMETER) != 0)
    {
        displayImage(&warning_tempsensor_16x16_streamimg,160-8-16-4-16,64);
    }
    else
    {
        fillSquare(&bgclr,160-8-16-4-16,64,16,16);
    }

}

void rootAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    uint8_t rotated=0;
    if (encoderIncr > 1 && ctx.entrySelected<3)
    {
        ctx.entrySelected++;
        rotated=1;
    }
    else if (encoderIncr < -1 && ctx.entrySelected > 1)
    {
        ctx.entrySelected--;
        rotated=1;
    }

    if (rotated == 1)
    {
        if(ctx.entrySelected==1)
        {
            drawSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y);
            clearSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y);
            clearSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y);
            //fillSquare(&entrySel,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
            //fillSquare(&bgclr,ROOT_ICON_2_X-2,ROOT_ICON_Y-2,32+4,32+4);
            //fillSquare(&bgclr,ROOT_ICON_3_X-2,ROOT_ICON_Y-2,32+4,32+4);
        }
        else if(ctx.entrySelected==2)
        {
            clearSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y);
            drawSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y);
            clearSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y);
            //fillSquare(&bgclr,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
            //fillSquare(&entrySel,ROOT_ICON_2_X-2,ROOT_ICON_Y-2,32+4,32+4);
            //fillSquare(&bgclr,ROOT_ICON_3_X-2,ROOT_ICON_Y-2,32+4,32+4);
        }
        else if(ctx.entrySelected==3)
        {
            clearSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y);
            clearSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y);
            drawSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y);
            //fillSquare(&bgclr,ROOT_ICON_1_X-2,ROOT_ICON_Y-2,32+4,32+4);
            //fillSquare(&bgclr,ROOT_ICON_2_X-2,ROOT_ICON_Y-2,32+4,32+4);
            //fillSquare(&entrySel,ROOT_ICON_3_X-2,ROOT_ICON_Y-2,32+4,32+4);
        }
        //displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
        //displayImage(&clock_32x32_streamimg,ROOT_ICON_2_X,ROOT_ICON_Y);
        //displayImage(&gearwheel_32x32_streamimg,ROOT_ICON_3_X,ROOT_ICON_Y);
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
    timeToString(ctx.timeStr,getHour(),getMinute(),getSecond());
    (app+index)->data=NULL;
    (app+index)->display = &rootAppDisplay;
    (app+index)->encoderSwitchCallback = &rootAppEncoderSwitchCallback;
    (app+index)->loop=&rootAppLoop;
}

