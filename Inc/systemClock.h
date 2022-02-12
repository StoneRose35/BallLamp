/*
 * systemClock.h
 *
 *  Created on: Aug 22, 2021
 *      Author: philipp
 */
#ifndef SYSTEMCLOCK_H_
#define SYSTEMCLOCK_H_

#include "systemChoice.h"
#include <stdint.h>
#ifdef HARDWARE

#ifdef RP2040_FEATHER

#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define XOSC_STARTUP ((volatile uint32_t*)(XOSC_BASE+XOSC_STARTUP_OFFSET))
#define XOSC_CTRL_ENABLE ((volatile uint32_t*)(XOSC_BASE+XOSC_CTRL_OFFSET))
#define XOSC_STATUS ((volatile uint32_t*)(XOSC_BASE+XOSC_STATUS_OFFSET))

#define CLK_SYS_CTRL ((volatile uint32_t*)(CLOCKS_BASE+CLOCKS_CLK_SYS_CTRL_OFFSET))
#define CLK_REF_CTRL ((volatile uint32_t*)(CLOCKS_BASE+CLOCKS_CLK_REF_CTRL_OFFSET))
#define CLK_PERI_CTRL ((volatile uint32_t*)(CLOCKS_BASE+CLOCKS_CLK_PERI_CTRL_OFFSET))

typedef struct {
	volatile uint32_t cs;
	volatile uint32_t pwr;
	volatile uint32_t fbdiv;
	volatile uint32_t prim;
} PllType;

#define PLL_SYS ((PllType*)PLL_SYS_BASE)

#define PLL_USB ((PllType*)PLL_USB_BASE)

#endif

void setupClock();
void initUsbPll();

#endif /* SYSTEMCLOCK_H_ */

#endif
