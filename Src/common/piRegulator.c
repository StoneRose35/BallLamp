
#include "piRegulator.h"
#include "services/triopsBreederService.h"

uint16_t getRegulatedHeaterValue(TriopsControllerType * controller,uint16_t measuredTemp)
{
    int32_t interm;
        // set  heater: h_max - (t_new-t_lower)/(t_target-t_lower)*h_max + cIntergral*intergratedTempDiff
    // max if t_new < t_lower, 0 if t_new > t_target
    interm = (measuredTemp - controller->tLower)*(1024 << 4);
    interm = (1024 << 4) -  interm/(controller->tTarget - controller->tLower) + controller->integralTempDeviation*controller->cIntegral;
    if (((measuredTemp < controller->tLower) && interm > (1023 << 4))  || (measuredTemp > controller->tTarget && interm < 0)) 
    // reset integral temperature deviation if outside of regulation range 
    {
        controller->integralTempDeviation=0;
    }
    else
    {
        controller->integralTempDeviation = controller->integralTempDeviation + (controller->tTarget-measuredTemp) ;
    }

    if (interm < 0)
    {
        controller->heaterValue = 0;
        return 0;
    }
    else if (interm > (1023 << 4))
    {
        controller->heaterValue = 1023;
        return 1023;
    }
    else
    {
        controller->heaterValue = ((uint32_t)interm) >> 4;
        return (uint16_t)(((uint32_t)interm) >> 4);
    }
    return 0;
}