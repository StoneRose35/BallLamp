#include "apps/voidApp.h"
#include "imgDisplay.h"
#include "charDisplay.h"
#include "stringFunctions.h"

static struct voidAppContext
{
    uint8_t page;
} ctx;

void voidAppEncoderSwitchCallback(int16_t encoderIncr,int8_t switchChange)
{
    if (switchChange != 0)
    {
        setPagePtr(0);
        display();
    }
}

void voidAppDisplay()
{
    RGB bgclr;
    char nrbfr[4];
    bgclr.r = 0;
    bgclr.g = 0;
    bgclr.b = 0;
    fillSquare(&bgclr,0,0,160,128);
    writeText("Void ",80-32,64-8,FONT_TYPE_16X16);
    UInt8ToChar(ctx.page,nrbfr);
    writeText(nrbfr,80-32 + 5*16,64-8,FONT_TYPE_16X16 );
}

void createVoidApp(SubApplicationType* app,uint8_t index)
{
    ctx.page = index;
    (app+index)->encoderSwitchCallback=&voidAppEncoderSwitchCallback;
    (app+index)->display=&voidAppDisplay;
}