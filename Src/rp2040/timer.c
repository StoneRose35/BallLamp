#include "timer.h"

void initTimer()
{
    // de-reset adc block
    *RESETS |= (1 << RESETS_RESET_TIMER_LSB);
    *RESETS &= ~(1 << RESETS_RESET_TIMER_LSB);
    while ((*RESETS_DONE & (1 << RESETS_RESET_TIMER_LSB)) == 0);

    *TIMER_TIMELW = 0;
    *TIMER_TIMEHW = 0;
}

void resetTime()
{
    *TIMER_TIMELW = 0;
    *TIMER_TIMEHW = 0;
}

uint32_t getTimeLW()
{
    return *TIMER_TIMELR;
}