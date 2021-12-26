
#include "systick.h"
#include "system.h"

uint32_t ticks=0;

void isr_systick()
{
    ticks++;
}

uint32_t getTickValue()
{
    return ticks;
}

void initSystickTimer()
{
    *M0PLUS_SYST_CVR = F_SYS/100;
    *M0PLUS_SYST_RVR = F_SYS/100; // generate a systick every 10ms
    *M0PLUS_SYST_CSR = (1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB) | (1 << M0PLUS_SYST_CSR_ENABLE_LSB) | (1 << M0PLUS_SYST_CSR_TICKINT_LSB);
}


void stopSystickTimer()
{
    *M0PLUS_SYST_CVR = F_SYS/100;
    *M0PLUS_SYST_RVR = F_SYS/100;
    *M0PLUS_SYST_CSR &= ~(1 << M0PLUS_SYST_CSR_ENABLE_LSB);
}
