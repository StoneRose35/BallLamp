
#include "displayCommands.h"
#include "spi_sdcard_display.h"
#include "uart.h"

void initDisplayCommand(char * cmd,void* context)
{
    printf("Initializing Display .. ");
    initDisplay(); 
    printf("Done \r\n");
}