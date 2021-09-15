/*
 * systemClock.c
 *
 *  Created on: 21.08.2021
 *      Author: philipp
 */

#include <neopixelDriver.h>

void setupClock()
{
	// set flash access to 2 wait states
	FLASH->ACR |= (2 << LATENCY);

	// switch clock to HSI
	RCC->CFGR &= ~(3 << SW);
	while((RCC->CFGR & (3 << SWS)) != 0)
	{}

	// disable PLL (if it was on)
	RCC->CR &= ~(1 << PLLON);
	while ((RCC->CR & (1 << PLLRDY)) == (1 << PLLRDY))
	{}

	RCC->CFGR |= (0xF << PLLMULT); // multiply by 16 resulting in 8/2*16 = 64 MHz

	// enable PLL
	RCC->CR |= (1 << PLLON);
	while ((RCC->CR & (1 << PLLRDY)) != (1 << PLLRDY))
	{}

	// set APB2 prescaler to 1 to get a APB1 bus clock of 32 MHz
	RCC->CFGR |= 4 << PPRE1;

	// switch clock source to pll
	RCC->CFGR |= (2 << SW);
	while((RCC->CFGR & (2 << SWS)) != (2 << SWS))
	{}

}
