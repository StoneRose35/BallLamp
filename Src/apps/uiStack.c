#include <stdint.h>
#include "uiStack.h"
#include "rotaryEncoder.h"
#include "apps/rootApp.h"
#include "apps/voidApp.h"
#include "apps/setDateTimeApp.h"
#include "systick.h"

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
    
    if (encoderIncr != 0 || switchChange != 0)
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
