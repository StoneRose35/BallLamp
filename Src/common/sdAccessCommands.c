
#include "sdAccessCommands.h"
#include <stdint.h>
#include "uart.h"
#include "spi_sdcard_display.h"
#include "stringFunctions.h"
#include "fatLib.h"
static uint8_t card_init = 0;

void sdInitCommand(char * cmd,void* context)
{
    char nrbfr[4];
    uint8_t retcode;
    if (card_init == 0)
    {
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
        //card_init = 1;
    }
    
    printf("\r\n");
    printf("mounting first FAT32 Partition\r\n");
    retcode = initFatSDCard();
    if (retcode > 0)
    {
        UInt8ToChar(retcode,nrbfr);
        printf(nrbfr);
    }
    else
    {
        printf("OK (0)");
    }
    
}

void mountCommand(char * cmd,void* context)
{
    char nrbfr[4];
    uint8_t retcode;
    printf("\r\n");
    printf("mounting first FAT32 Partition\r\n");
    retcode = initFatSDCard();
    if (retcode > 0)
    {
        UInt8ToChar(retcode,nrbfr);
        printf(nrbfr);
    }
    else
    {
        printf("OK (0)");
    }
}