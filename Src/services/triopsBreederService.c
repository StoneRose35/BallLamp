#include "ds18b20.h"
#include "services/triopsBreederService.h"
#include "datetimeClock.h"
#include "remoteSwitch.h"
#include "stringFunctions.h"
#include "fatLib.h"
#include "heater.h"
#include "systick.h"
#include "piRegulator.h"

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
    triopsController.cIntegral=10; // 0.625
    triopsController.integralDampingFactor=16; // 1
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
    triopsController.totalMinutesOn = triopsController.minuteOn + 60*triopsController.hourOn;
    triopsController.totalMinutesOff = triopsController.minuteOff + 60*triopsController.hourOff;
    triopsController.serviceInterval = 3000; // default interval is 30s (3000 ticks)
    triopsController.errorFlags = 0; // no error before initialization
     
    // open log file
    createDirectoryPointer(&pdir);
    createDirectoryPointer(&cdir);
    retcode = openRootDirectory(pdir);
    if(retcode > 0) { triopsController.errorFlags |= TC_ERROR_FILESYSTEM; return retcode; }
    retcode = openDirectory(pdir,"LOGS",cdir);
    if(retcode > 0 ) {triopsController.errorFlags |= TC_ERROR_FILESYSTEM; return retcode; }
    copyDirectoryPointer(&cdir,&pdir);
    retcode = openFile(pdir,"triops.log",&fp);
    if (retcode == FATLIB_FILE_NOT_FOUND)
    {
        retcode = createFile(pdir,"triops.log");
        if (retcode > 0) { triopsController.errorFlags |= TC_ERROR_FILESYSTEM; return retcode; }
        retcode = openFile(pdir,"triops.log",&fp);
        if (retcode > 0) { triopsController.errorFlags |= TC_ERROR_FILESYSTEM; return retcode; }
    }
    retcode = seekEnd(&fp);
    if (retcode > 0) { triopsController.errorFlags |= TC_ERROR_FILESYSTEM; return retcode; }
    oldTicks = getTickValue();
    return 0;
}

void triopBreederServiceLoop()
{
    int16_t currentTemp;
    uint8_t retcode, rc2;
    //int32_t interm;
    char logLine[64];
    char nrbfr[16];
    uint16_t strLen;
    uint16_t totalMinutes;

    triopsController.errorFlags = 0;

    //interval defined on the data structure
    if ((triopsController.serviceInterval > 0 && getTickValue() > oldTicks + triopsController.serviceInterval) || getTempReadState() == 1)
    {

        // get temperature reading 
        if (getTempReadState() == 1)
        {
            retcode = readTemp(&currentTemp);
            if (retcode== 2) // start a new conversion if none has started yet
            {
                rc2 = initTempConversion();
                if (rc2 > 0)
                {
                    triopsController.errorFlags |= TC_ERROR_THERMOMETER;
                }
            }
            else if (retcode == 1)
            {
                triopsController.errorFlags |= TC_ERROR_THERMOMETER;
            }
        }
        else
        {
            rc2 = initTempConversion();
            if (rc2 !=0)
            {
                triopsController.errorFlags |= TC_ERROR_THERMOMETER;
            }
            retcode = 3;    
        }

        if (retcode == 0)
        {
            getRegulatedHeaterValue(&triopsController,currentTemp);
            setHeater(triopsController.heaterValue);
            triopsController.temperature = currentTemp;


            // if time  > t_start and lamp is off: turn lamp on
            totalMinutes = getHour()*60 + getMinute();
            if ((totalMinutes >= triopsController.totalMinutesOn) && 
                (totalMinutes < triopsController.totalMinutesOff))
            {
                triopsController.lampState = 1;
                remoteSwitchOn();
            }
            else
            {
                triopsController.lampState = 0;
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
            retcode = appendToFile(pdir,&fp,(uint8_t*)logLine,strLen);
            if (retcode > 0)
            {
                triopsController.errorFlags |= TC_ERROR_FILESYSTEM;
            }
            oldTicks=getTickValue();
        }
    }
}

void stopTriopsBreederService()
{
    writeFile(pdir,&fp,fp.sectorBufferPtr);

}