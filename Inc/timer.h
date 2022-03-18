#ifndef _TIMER_H_
#define _TIMER_H_
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/resets.h"

#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define TIMER_TIMEHW ((volatile uint32_t*)(TIMER_BASE + TIMER_TIMEHW_OFFSET))
#define TIMER_TIMELW ((volatile uint32_t*)(TIMER_BASE + TIMER_TIMELW_OFFSET))
#define TIMER_TIMEHR ((volatile uint32_t*)(TIMER_BASE + TIMER_TIMEHR_OFFSET))
#define TIMER_TIMELR ((volatile uint32_t*)(TIMER_BASE + TIMER_TIMELR_OFFSET))
void initTimer();

void resetTime();

uint32_t getTimeLW();

#endif