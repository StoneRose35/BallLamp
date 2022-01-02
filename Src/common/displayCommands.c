
#include "displayCommands.h"
#include "spi_sdcard_display.h"
#include "uart.h"
#include "stringFunctions.h"
#include "taskManagerUtils.h" 

void initDisplayCommand(char * cmd,void* context)
{
    printf("Initializing Display .. ");
    initDisplay(); 
    printf("Done \r\n");
}

void setBacklightCommand(char * cmd,void* context)
{
    char bcontent[8];
    uint8_t brightnessval, errorflag = 0;
    getBracketContent(cmd,bcontent);
    brightnessval = tryToUInt8(bcontent,&errorflag);
    if(errorflag == 0)
    {
        setBacklight(brightnessval);
    }
    else{
        printf("\r\nERROR: brightness value must be an integer from 0 to 255\r\n");
    }
}