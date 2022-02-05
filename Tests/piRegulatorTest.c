#include "piRegulator.h"
#include <stdlib.h>
#include <stdio.h>
#include "stringFunctions.h"

static uint16_t heaterHistory[8];
static const uint16_t heaterHistCoeff[8]={
    4, 4, 4, 4,
    0, 0, 0, 0
};

void initSimulator()
{
    for(uint8_t c=0;c<8;c++)
    {
        heaterHistory[c]=0;
    }
}

uint16_t getSimulatedTemperature(TriopsControllerType * controller)
{
    uint32_t interm_temp=0;
    uint32_t ntemp;
    for (uint8_t c=7;c>0;c--)
    {
        heaterHistory[c] = heaterHistory[c-1];
    }
    heaterHistory[0] = controller->heaterValue;

    for(uint8_t c=0;c<8;c++)
    {
        interm_temp +=heaterHistory[c]*heaterHistCoeff[c];
    }
    interm_temp >>= 4;
    // 19.0 + heater/562*(25.5-19.0)
    ntemp = 19 << 4;
    interm_temp = (interm_temp*(((25<< 4)+8) - (19 << 4))) >> 4;
    interm_temp = (interm_temp << 4)/562;
    ntemp += interm_temp;
    return (uint16_t)ntemp;
}

void main(int argc,char** argv)
{  
    TriopsControllerType tc;
    uint16_t tSim;
    FILE * fid;
    char nrbfr[16];
    tc.integralDampingFactor = 16;
    tc.integralTempDeviation = 0;
    tc.tTarget = (25<<4) + 4;
    tc.tLower = (20<<4);
    tc.cIntegral = 30;
    tc.heaterValue = 0;
    initSimulator();
    fid = fopen("piregulator.txt","wt");
    for(uint16_t c=0;c<128;c++)
    {
        tSim = getSimulatedTemperature(&tc);
        getRegulatedHeaterValue(&tc,tSim);
        fixedPointUInt16ToChar(nrbfr,tSim,4);
        fprintf(fid,"%s, %d, %d\r\n",nrbfr,tc.heaterValue,tc.integralTempDeviation);
        printf("%s, %d, %d\r\n",nrbfr,tc.heaterValue,tc.integralTempDeviation);
    }
    fclose(fid);

}