/*
 * systemClock.c
 *
 *  Created on: 21.08.2021
 *      Author: philipp
 */
#ifdef RP2040_FEATHER

#include "hardware/regs/addressmap.h"
#include "hardware/regs/xosc.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/pll.h"

#include "neopixelDriver.h"
#include "types.h"


void setupClock()
{
	// explicitely define startup delay cycles
	// 281 results from (12MHz * 6ms)/256
	*XOSC_STARTUP = 281;
	*XOSC_CTRL_ENABLE = XOSC_CTRL_ENABLE_VALUE_ENABLE | XOSC_CTRL_FREQ_RANGE_VALUE_1_15MHZ;
	while ((*XOSC_STATUS | (1 << XOSC_STATUS_STABLE_LSB))==0);

	// switch to XOSC for ref 
	*CLK_REF_CTRL |= (0x02 << 0);

	//lauch PLL
	//refdiv 1
	PLL_SYS->cs |= (1 << 0) ;
	//vco runs at 12MHz*FEEDBK
	PLL_SYS->fbdiv = FEEDBK;

	// wwitch on pll itself and vco
	PLL_SYS->pwr &= ~((1 << PLL_PWR_PD_LSB) | (1 << PLL_PWR_VCOPD_LSB));

	//wait until lock is achieved
	while((PLL_SYS->cs & (1 << PLL_CS_LOCK_LSB))==0);

	// system clock runs at 1560MHz/12 =130 MHz
	PLL_SYS->prim = (2 << POSTDIV1) | (6 << POSTDIV2);

	// enable post divider
	PLL_SYS->pwr &= ~(1 << PLL_PWR_POSTDIVPD_LSB);

	// switch system clock to pll (src to aux which is sys pll by default)
	*CLK_SYS_CTRL |= (0x01 << 0);
}

#endif
