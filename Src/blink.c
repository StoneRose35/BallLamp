/*
 * blink.c
 *
 *  Created on: 21.08.2021
 *      Author: philipp
 */
#include <neopixelDriver.h>


#ifdef BLINK
void TIM2_IRQHandler()
{

	if ((TIM2->SR & 0x1) == 0x1) // update interrupt flag
	{

		if ((GPIOB->ODR & (1<<3)) == 1<<3)
		{
			GPIOB->ODR &= ~(1<<3);
		}
		else
		{
			GPIOB->ODR |= (1<<3);
		}
		TIM2->SR &= ~(0x1); // clear interrupt
	}

	return;
}
#endif

void initBlinkTimer()
{

    RCC->APB1ENR |= 1 << TIM2EN;

	// set timing
	TIM2->ARR =  F_BUS;
	TIM2->DIER |= 1; // update interrupt enable


	*NVIC_ISER0 |= (1 << 28);

	TIM2->CR1 |= 1;

}

void initLedPort()
{
    RCC->AHBENR |= (1 << IOPBEN); // enable gpio b
	GPIOB->MODER = (OUTPUT << 6) | (OUTPUT); // set PB3 as output

	GPIOB->ODR = (1<<3);
}

