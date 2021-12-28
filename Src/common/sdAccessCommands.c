
#include "sdAccessCommands.h"
#include <stdint.h>
#include "uart.h"
#include "spi_sdcard.h"
#include "stringFunctions.h"

void sdInitCommand(char * cmd,void* context)
{
    char nrbfr[4];
    uint8_t retcode;
    printf("\r\rInitializing SD Card..\r\nReturn Code is: ");
    retcode = initSdCard();
    if (retcode > 0)
    {
        UInt8ToChar(retcode,nrbfr);
        printf(nrbfr);
    }
    else
    {
        printf("OK (0)");
    }
    printf("\r\n");

}