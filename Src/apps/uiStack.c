#include <stdint.h>
#include "uiStack.h"
#include "rotaryEncoder.h"
#include "apps/rootApp.h"
#include "apps/voidApp.h"
#include "apps/setDateTimeApp.h"
#include "apps/configApp.h"
#include "systick.h"
#include "apps/uiColors.h"
#include "images/arrow_up_16x8.h"
#include "images/arrow_down_16x8.h"

static volatile uint32_t encoderVal=0;
static volatile uint8_t switchVal=0;
static volatile uint32_t pagePtr=0;
static volatile uint32_t cTick;



static SubApplicationType uiApplications[16];


void setPagePtr(uint32_t ptr)
{
    pagePtr=ptr;
}


void initUiStack()
{
    createRootApp(uiApplications,0);
    createSetDateTimeApp(uiApplications, 1);
    createVoidApp(uiApplications, 2);
    createConfigApp(uiApplications,3);
    cTick=getTickValue();
}

void uiStackTask(uint32_t task)
{
    uint32_t encoderValCurrent;
    uint8_t switchValCurrent;
    int16_t encoderIncr=0;
    int8_t switchChange=0;
    encoderValCurrent=getEncoderValue();
    switchValCurrent=getSwitchValue();


    encoderIncr = encoderValCurrent - encoderVal;
    switchChange = switchValCurrent - switchVal;
    
    if (encoderIncr > 1 || encoderIncr < -1 || switchChange != 0)
    {
        uiApplications[pagePtr].encoderSwitchCallback(encoderIncr,switchChange);
        encoderVal=encoderValCurrent;
        switchVal=switchValCurrent;
    }
    if(getTickValue() > cTick+2 && uiApplications[pagePtr].loop != 0)
    {
        uiApplications[pagePtr].loop(uiApplications[pagePtr].data);
        cTick=getTickValue();
    }
}

void display()
{
    uiApplications[pagePtr].display(uiApplications[pagePtr].data);
}



/**
 * @brief draws at 2 pixel tall bar spaced 16 pixel vertically apart
 * 
 * @param posx position in pixel
 * @param posy position in pixels
 * @param width width in pixels
 */
void drawSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width,uint8_t spacing)
{
    fillSquare(&entrySel,posx,posy-2,width,2);
    fillSquare(&entrySel,posx,posy+spacing,width,2);
}


void clearSelectMarkers(uint8_t posx,uint8_t posy,uint8_t width,uint8_t spacing)
{
    fillSquare(&bgclr,posx,posy-2,width,2);
    fillSquare(&bgclr,posx,posy+spacing,width,2);
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
