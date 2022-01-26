#include "imgDisplay.h"
#include "apps/uiColors.h"
#include "apps/draftHorseApp.h"
#include "images/kaltblut1.h"
#include "images/kaltblut2.h"
#include "images/kaltblut3.h"


static volatile int8_t encoderPos;


void createDraftHorseApp(SubApplicationType* app,uint8_t index)
{
    encoderPos = 0;
    (app + index)->display = &draftHorseDisplay;
    (app + index)->encoderSwitchCallback = &draftHorseAppEncoderSwitchCallback;
    (app + index)->data = (void*)0;
    (app + index)->loop = (void*)0;
}
void draftHorseDisplay(void* data)
{
    fillSquare(&bgclr,0,0,160,128);
    switch (encoderPos)
    {
        case 0:
            displayImage(&kaltblut1_streamimg,0,0);
            break;
        case 1:
            displayImage(&kaltblut2_streamimg,0,0);
            break;
        case 2:
            displayImage(&kaltblut3_streamimg,0,0);
            break;
    }
}
void draftHorseAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    if (encoderIncr != 0)
    {
        encoderPos += encoderIncr;
        if (encoderPos < 0)
        {
            encoderPos = 0;
        }
        else if (encoderPos > 2)
        {
            encoderPos = 2;
        }
        draftHorseDisplay((void*)0);
    }

    if (switchChange == -1)
    {
        setPagePtr(0);
        display();
    }
}