#include <stdint.h>
#include "uiStack.h"
#include "rotaryEncoder.h"
#include "apps/rootApp.h"
#include "apps/voidApp.h"

static volatile uint32_t encoderVal=0;
static volatile uint8_t switchVal=0;
static volatile uint32_t pagePtr=0;



static SubApplicationType uiApplications[16];


void setPagePtr(uint32_t ptr)
{
    pagePtr=ptr;
}


void initUiStack()
{
    createRootApp(uiApplications,0);
    createVoidApp(uiApplications, 1);
    createVoidApp(uiApplications, 2);
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
    }
}

void display()
{
    uiApplications[pagePtr].display();
}
