#ifndef _SYSTICK_
#define _SYSTICK_

#include "systemChoice.h"
#include <stdint.h>
#ifdef HARDWARE


#ifdef RP2040_FEATHER
#include "hardware/regs/addressmap.h"
#include "hardware/regs/m0plus.h" 

#define M0PLUS_SYST_CSR ((volatile uint32_t*)(PPB_BASE + M0PLUS_SYST_CSR_OFFSET))
#define M0PLUS_SYST_RVR ((volatile uint32_t*)(PPB_BASE + M0PLUS_SYST_RVR_OFFSET))
#define M0PLUS_SYST_CVR ((volatile uint32_t*)(PPB_BASE + M0PLUS_SYST_CVR_OFFSET))
#endif

#ifdef STM32
#define STK_CTRL ((volatile uint32_t*)0xE000E010U)
#define STK_LOAD ((volatile uint32_t*)0xE000E014U)
#define STK_VAL ((volatile uint32_t*)0xE000E018U)
#endif
/**
 * @brief returns the number of ticks passed since the systick-time was been switched on
 * divide this number by 100 to get the runtime in seconds. The tick value is stored in a 32bit unsigned integer
 * so an overflow is (theoretically) expected every 497.102 days
 * 
 * @return the tick value
 */
uint32_t getTickValue();
void initSystickTimer();
void stopSystickTimer();

#endif

#endif