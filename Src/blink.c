/*
 * blink.c
 *
 *  Created on: 21.08.2021
 *      Author: philipp
 */
#include "led_timer.h"


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

void initBlinkTimer()
{

    *RCC_APB1ENR |= 1 << TIM2EN;

    //TIM2->CCMR1 |= (6 << OC1M) | (1 << OC1PE) | (1 << OC1FE); // set PWM settings


	//GPIOA->AFRL |= 1 << PINA0POS; // AF1 of pina0 is TIM2_CH1

	// set timing
	TIM2->ARR =  0x044AA200;
	TIM2->DIER |= 1; // update interrupt enable

	// enable dma request on CCR1
	//TIM2->DIER |= 1 << UDE;

    // DMA configuration

    //set peripheral register to capture/compare register 1 of timer 2
    //DMA->CHANNEL[1].CPAR = (uint32_t)&(TIM2->CCR1);

    // set memory address to the raw data pointer
    //DMA->CHANNEL[1].CMAR = (uint32_t)rawdata_ptr;

    // set number of bytes to transfer
    //DMA->CHANNEL[1].CNDTR=80*24;

    //DMA->CHANNEL[1].CCR |= (3 << PL) | (2 << MSIZE) | (2 << PSIZE) | (1 << MINC) | (1 << DIR) | (1 << TCIE);
    //DMA->CHANNEL[1].CCR |= (1 << EN);

	*NVIC_ISER0 |= (1 << 28);

	TIM2->CR1 |= 1;

}

void initLedPort()
{
    *RCC_AHBENR |= (1 << 18); // enable gpio b
	GPIOB->MODER = (OUTPUT << 6) | (OUTPUT); // set PB3 as output

	GPIOB->ODR = (1<<3);
}

