#include "ds18b20.h"
#include "services/triopsBreederService.h"
#include "datetimeClock.h"
#include "remoteSwitch.h"
#include "stringFunctions.h"
#include "fatLib.h"
#include "heater.h"
#include "systick.h"

static volatile uint32_t heaterVal;
static volatile uint32_t oldTicks;
static TriopsControllerType triopsController;
static DirectoryPointerType * pdir;
static DirectoryPointerType * cdir;
static FilePointerType fp;

TriopsControllerType * getTriopsController()
{
    return &triopsController;
}


uint8_t initTriopBreederService()
{
    uint8_t retcode;
    triopsController.cIntegral=10;
    triopsController.heaterValue=0;
    triopsController.tLower = 20 << 4;
    triopsController.tTarget = 25 << 4 | 4; // 25.25
    triopsController.temperature= (20 << 4) | 8; // 20.5
    triopsController.lampState = 0;
    triopsController.integralTempDeviation = 0;
    triopsController.hourOn = 6;
    triopsController.minuteOn = 12;
    triopsController.hourOff = 20;
    triopsController.minuteOff = 22;
    triopsController.serviceInterval = 3000; // default interval is 30s (3000 ticks)
     
    // open log file
    createDirectoryPointer(&pdir);
    createDirectoryPointer(&cdir);
    retcode = openRootDirectory(pdir);
    if(retcode > 0) { return retcode; }
    retcode = openDirectory(pdir,"LOGS",cdir);
    if(retcode > 0 ) {return retcode; }
    copyDirectoryPointer(&cdir,&pdir);
    retcode = openFile(pdir,"triops.log",&fp);
    if (retcode == FATLIB_FILE_NOT_FOUND)
    {
        retcode = createFile(pdir,"triops.log");
        if (retcode > 0) { return retcode; }
        retcode = openFile(pdir,"triops.log",&fp);
        if (retcode > 0) { return retcode; }
    }
    seekEnd(&fp);
    oldTicks = getTickValue();
    return 0;
}

void triopBreederServiceLoop()
{
    int16_t currentTemp;
    uint8_t retcode;
    int32_t interm;
    char logLine[64];
    char nrbfr[16];
    uint16_t strLen;

    //interval defined on the data structure
    if (triopsController.serviceInterval > 0 && getTickValue() > oldTicks + triopsController.serviceInterval)
    {

        // get temperature reading 
        #ifndef TB_MOCK
            retcode = readTemp(&currentTemp);
            if (retcode == 0 || retcode== 2) // start a new conversion if temerature could be read successfully or if no conversion has started yet
            {
                initTempConversion();
            }
        #else
            retcode = 0;
            currentTemp = (22 << 4) | 8;
        #endif


        

        // set  heater: h_max - (t_new-t_lower)/(t_target-t_lower)*h_max + cIntergral*intergratedTempDiff
        // max if t_new < t_lower, 0 if t_new > t_target
        interm = (currentTemp - triopsController.tLower)*(1024 << 4);
        interm = (1024 << 4) -  interm/(triopsController.tTarget - triopsController.tLower) + triopsController.integralTempDeviation*triopsController.cIntegral;
        triopsController.integralTempDeviation += triopsController.tTarget-currentTemp ;
        if (interm < 0)
        {
            triopsController.heaterValue = 0;
        }
        else if (interm > (1024 << 4))
        {
            triopsController.heaterValue = 1024;
        }
        else
        {
            triopsController.heaterValue = ((uint32_t)interm) >> 4;
        }
        setHeater(triopsController.heaterValue);
        triopsController.temperature = currentTemp;


        // if time  > t_start and lamp is off: turn lamp on
        if ((getHour() >= triopsController.hourOn && getMinute() >= triopsController.minuteOn) && 
            (getHour() < triopsController.hourOff && getMinute() < triopsController.minuteOff))
        {
            remoteSwitchOn();
        }
        else
        {
            remoteSwitchOff();
        }
        dateTimeToString(logLine,getYear(),getMonth(),getDay(),getHour(),getMinute(),getSecond());
        appendToString(logLine, ", ");
        UInt16ToChar(triopsController.heaterValue,nrbfr);
        appendToString(logLine,nrbfr);
        appendToString(logLine, ", ");
        fixedPointUInt16ToChar(nrbfr,triopsController.temperature,4);
        appendToString(logLine,nrbfr);
        strLen = appendToString(logLine,"\r\n");    
        appendToFile(pdir,&fp,(uint8_t*)logLine,strLen);
        oldTicks=getTickValue();
    }
}

void stopTriopsBreederService()
{
    writeFile(pdir,&fp,fp.sectorBufferPtr);

}