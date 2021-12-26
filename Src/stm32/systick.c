
#include "systick.h"
#include "system.h"

uint32_t ticks=0;

void SysTick_Handler()
{
    ticks++;
}

uint32_t getTickValue()
{
    return ticks;
}

void initSystickTimer()
{
    *STK_VAL = F_SYS/100;
    *STK_LOAD = F_SYS/100; // generate a systick every 10ms
    *STK_CTRL = (1 << 2) | (1 << 0) | (1 << 1); // clock from processor/ahb, enable systick clock, enable systick interrupt
}


void stopSystickTimer()
{
    *STK_VAL = F_SYS/100;
    *STK_LOAD = F_SYS/100;
    *STK_CTRL &= ~(1 << 0); // reset systick enable
}
