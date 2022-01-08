
#include "sdAccessCommands.h"
#include <stdint.h>
#include "uart.h"
#include "spi_sdcard_display.h"
#include "stringFunctions.h"
#include "fatLib.h"
#include "consoleHandler.h"
static uint8_t card_init = 0;

extern DirectoryPointerType * cwd;
extern DirectoryPointerType * ndir;

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

void cdCommand(char * cmd,void * context)
{
    char dirname[16];
    char nrbfr[4];
    uint8_t retcode=0;
    getBracketContent(cmd,dirname);
    switch(validateDirName(dirname))
    {
        case 0:
        retcode = openDirectory(cwd,dirname,ndir);
        if (retcode != 0)
        {
            printf("\r\nfailure opening directory: ");
            UInt8ToChar(retcode,nrbfr);
            printf(nrbfr);
            printf("\r\n");
            return;
        }
        retcode = copyDirectoryPointer(&ndir,&cwd);
        if (retcode != 0)
        {
            printf("failure copying directory pointers: ");
            UInt8ToChar(retcode,nrbfr);
            printf(nrbfr);
            printf("\r\n");
            return;
        }
        if (dirname[0]=='.' && dirname[1]=='.')
        {
            removeLastPath();
        }
        else
        {
            addToPath(dirname);
        }
        break;
        case SDACCESS_INVALID_DIRNAME:
            printf("\r\nInvalid Directory Name\r\n");
            break;
        case SDACCESS_DIRNAME_TOO_LONG:
            printf("\r\nDirectory name too long, only 8.3-Format supported\r\n");
    }
}

void lsCommand(char * cmd,void * context)
{
    char nrbfr[16];
    char displayLine[16];
    uint16_t c=0;
    uint8_t dcnt=0;
    for(c=0; c<cwd->entriesLength;c++){
        dcnt = displayFilename(*cwd->entries + c,displayLine);
        while(dcnt < 15)
        {
            displayLine[dcnt++]=' ';
        }
        displayLine[dcnt] = 0;
        printf(displayLine);
        printf("Size: ");
        UInt32ToChar((*cwd->entries + c)->size,nrbfr);
        printf(nrbfr);
        printf("\r\n");
    } 
}

void mkdirCommand(char * cmd,void * context)
{
    char dirname[16];
    char nrbfr[4];
    uint8_t retcode=0;
    getBracketContent(cmd,dirname);
    switch(validateDirName(dirname))
    {
        case 0:
        retcode = createDirectory(cwd,dirname);
        if (retcode != 0)
        {
            printf("\r\nfailure creating directory: ");
            UInt8ToChar(retcode,nrbfr);
            printf(nrbfr);
            printf("\r\n");
        }
        break;
        case SDACCESS_INVALID_DIRNAME:
            printf("\r\nInvalid Directory Name\r\n");
            break;
        case SDACCESS_DIRNAME_TOO_LONG:
            printf("\r\nDirectory name too long, only 8.3-Format supported\r\n");
    }
}

void rmdirCommand(char * cmd,void * context)
{
    char dirname[16];
    char nrbfr[4];
    uint8_t retcode=0;
    getBracketContent(cmd,dirname);
    switch(validateDirName(dirname))
    {
        case 0:
        retcode = openDirectory(cwd,dirname,ndir);
        if (retcode != 0)
        {
            printf("\r\nfailure removing directory, getting directory pointer: ");
            UInt8ToChar(retcode,nrbfr);
            printf(nrbfr);
            printf("\r\n");
            return;
        }
        retcode = deleteDirectory(cwd,ndir);
        if (retcode != 0)
        {
            printf("\r\nfailure removing directory: ");
            UInt8ToChar(retcode,nrbfr);
            printf(nrbfr);
            printf("\r\n");
        }
        break;
        case SDACCESS_INVALID_DIRNAME:
            printf("\r\nInvalid Directory Name\r\n");
            break;
        case SDACCESS_DIRNAME_TOO_LONG:
            printf("\r\nDirectory name too long, only 8.3-Format supported\r\n");
    }
}

uint8_t validateDirName(char *dirName)
{
    uint8_t c=0;
    while(*(dirName + c)!= 0)
    {
        if (*((uint8_t*)dirName + c) == '"' 
        || *((uint8_t*)dirName + c) == '*'
        || *((uint8_t*)dirName + c) == '/'
        || *((uint8_t*)dirName + c) == ':'
        || *((uint8_t*)dirName + c) == '<'
        || *((uint8_t*)dirName + c) == '>'
        || *((uint8_t*)dirName + c) == '?'
        || *((uint8_t*)dirName + c) == '\\'
        || *((uint8_t*)dirName + c) == '|'
        || *((uint8_t*)dirName + c) == '+'
        || *((uint8_t*)dirName + c) == ','
        || *((uint8_t*)dirName + c) == '.'
        || *((uint8_t*)dirName + c) == ';'
        || *((uint8_t*)dirName + c) == '='
        || *((uint8_t*)dirName + c) == '['
        || *((uint8_t*)dirName + c) == ']'
            )
        {
            return SDACCESS_INVALID_DIRNAME;
        }
        c++;
        if (c > 8)
        {
            return SDACCESS_DIRNAME_TOO_LONG;
        }
    }
    return 0;
}

uint8_t displayFilename(DirectoryEntryType * entry,char * res)
{
    uint8_t fcnt=0;
    for(uint8_t c=0;c<8;c++)
    {
        if(entry->filename[c] != ' ')
        {
            *(res+fcnt++) = entry->filename[c];
        }
    }
    if (entry->attrib == 0x20)
    {
        *(res+fcnt++) = '.';
        for(uint8_t c=0;c<3;c++)
        {
            if(entry->fileext[c] != ' ')
            {
                *(res+fcnt++) = entry->fileext[c];
            }
        }
    }
    return fcnt;
}
