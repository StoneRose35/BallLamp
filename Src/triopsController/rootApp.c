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
#include "images/recording_light_24x24.h"
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
    char tempString[16];
    uint32_t heaterBarWidth;
    uint32_t brightnessBarWidth;
    TriopsControllerType* triopsController = getTriopsController();
    if(getTickValue() >ctx.ticksLast+99)
    {
        // display the heater bar
        heaterBarWidth = ((triopsController->heaterValue*(80-8)) >> 10) & 0xFF;
        fillSquare(&heaterClr,0,20,(uint8_t)heaterBarWidth,8);
        fillSquare(&bgclr,(uint8_t)heaterBarWidth+1,20,80-(uint8_t)heaterBarWidth,8);

        brightnessBarWidth = ((triopsController->currentBrightness *(80-8)) >> 12) & 0xFF;
        fillSquare(&brightnessClr,80,20,(uint8_t)brightnessBarWidth,8);
        fillSquare(&bgclr,(uint8_t)brightnessBarWidth+80+1,20,80-(uint8_t)brightnessBarWidth,8);

        // display the temperature
        fixedPointUInt16ToChar(tempString,triopsController->temperature,4);
        writeText("T:",0,5,FONT_TYPE_8X8);
        writeText(tempString,2,5,FONT_TYPE_8X8);

        // display the integrated temperature deviation
        fixedPointUInt16ToChar(tempString,triopsController->integralTempDeviation,4);
        writeText("I:",0,6,FONT_TYPE_8X8);
        fillSquare(&bgclr,7*8,6*8,12*8,8);
        writeText(tempString,2,6,FONT_TYPE_8X8);    

        timeToString(ctx.timeStr,getHour(),getMinute(),getSecond());
        // display the time on top
        writeText(ctx.timeStr,16,0,FONT_TYPE_16X16);
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

        // display the recording button (if on)
        if (triopsController->serviceInterval > 0)
        {
            displayImage(&recording_light_24x24_streamimg,8+24+8,64);
        }
        else
        {
            fillSquare(&bgclr,8+24+8,64,24,24);
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
    uint32_t brightnessBarWidth;
    uint32_t brightnessThreshholdPosition;
    // draw background
    fillSquare(&bgclr,0,0,160,128);
    TriopsControllerType* triopsController = getTriopsController();
    // draw bottom icons
    displayImage(&drafthorse_32x32_streamimg,ROOT_ICON_1_X,ROOT_ICON_Y);
    displayImage(&clock_32x32_streamimg,ROOT_ICON_2_X,ROOT_ICON_Y);
    displayImage(&gearwheel_32x32_streamimg,ROOT_ICON_3_X,ROOT_ICON_Y);
    switch (ctx.entrySelected)
    {
        case 3:
            drawSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y+2);
            break;
        case 2:
            drawSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y+2);
            break;
        case 1:
            drawSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y+2);
            break;
    }

    
    // display the time on top
    writeText(ctx.timeStr,16,0,FONT_TYPE_16X16);

    // display the heater level
    // heaterLevel*(80-8)/1024
    // ------------------------------
    // |       |        |        |        |
    // |                |                 |
    heaterBarWidth = ((triopsController->heaterValue*(80-8)) >> 10) & 0xFF;
    fillSquare(&heaterClr,0,20,(uint8_t)heaterBarWidth,8);
    fillSquare(&clrBlack,0,28,1,4);
    fillSquare(&clrBlack,0+(80-8)/2,28,1,4);
    fillSquare(&clrBlack,0+(80-8),28,1,4);
    fillSquare(&clrBlack,0+(80-8)/4,28,1,2);
    fillSquare(&clrBlack,0+(80-8)*3/4,28,1,2);

    //Display the measured brightness
    brightnessBarWidth = ((triopsController->currentBrightness *(80-8)) >> 12) & 0xFF;
    brightnessThreshholdPosition = ((triopsController->brightnessThreshhold *(80-8)) >> 12) & 0xFF;
    fillSquare(&brightnessClr,80,20,(uint8_t)brightnessBarWidth,8);
    fillSquare(&clrBlack,80,28,1,4);
    fillSquare(&clrBlack,80+(80-8),28,1,4);
    fillSquare(&clrBlue,80+(uint8_t)brightnessThreshholdPosition,28,2,4);

    // display the temperature
    fixedPointUInt16ToChar(tempString,triopsController->temperature,4);
    writeText("T:",0,5,FONT_TYPE_8X8);
    writeText(tempString,2,5,FONT_TYPE_8X8);

    // display the integrated temperature deviation
    fixedPointUInt16ToChar(tempString,triopsController->integralTempDeviation,4);
    writeText("I:",0,6,FONT_TYPE_8X8);
    writeText(tempString,2,6,FONT_TYPE_8X8);    

    // display the lamp state
    if(triopsController->lampState == 0)
    {
        displayImage(&bulb_off_24x24_streamimg,8,64);
    }
    else
    {
        displayImage(&bulb_on_24x24_streamimg,8,64);
    }

    // display the recording button (if on)
    if (triopsController->serviceInterval > 0)
    {
        displayImage(&recording_light_24x24_streamimg,8+24+8,64);
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
            drawSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y+2);
            clearSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y+2);
            clearSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y+2);
        }
        else if(ctx.entrySelected==2)
        {
            clearSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y+2);
            drawSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y+2);
            clearSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y+2);
        }
        else if(ctx.entrySelected==3)
        {
            clearSelectFrame(ROOT_ICON_1_X,ROOT_ICON_Y+2);
            clearSelectFrame(ROOT_ICON_2_X,ROOT_ICON_Y+2);
            drawSelectFrame(ROOT_ICON_3_X,ROOT_ICON_Y+2);
        }
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

