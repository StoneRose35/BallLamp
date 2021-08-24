/*
 * led_timer.c
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */
#include "led_timer.h"

#ifndef BLINK
void TIM2_IRQHandler()
{
	if ((TIM2->SR & 0x1) == 0x1) // update interrupt flag
	{
		//re-enable timer to ensure a refresh rate of 1/30
		if (TIM2->ARR > WS2818_CNT)
		{
			// waited the remaining time of 1/30s
			TIM2->CR1 &= ~(1);
		}
		else
		{
			// finished clocking of the last data bit
			TIM2->ARR = 1979733;
			TIM2->CCR1 = 0x0;
			// disable capture compare
			TIM2->CCER &= ~1;
		}

		TIM2->SR &= ~(0x1); // clear interrupt
	}
}
#endif


void decompressRgbArray(RGBStream * frame,uint8_t length)
{
	uint8_t cnt = 0;
	uint8_t cbfr;
	uint16_t rdata_cnt = 0;
	*(rawdata_ptr + rdata_cnt) = 0;
	rdata_cnt=1;
	for(cnt=0;cnt<length;cnt++)
	{
		// refer to specific hardware implementation to check which color comes first
		cbfr = (*(frame+cnt)).rgb.g;
		for(uint8_t c=0;c<8;c++)
		{
			if((cbfr & 0x80) == 0x80)
			{
				*(rawdata_ptr + rdata_cnt) = WS2818_LONG_CNT;
			}
			else
			{
				*(rawdata_ptr + rdata_cnt) = WS2818_SHORT_CNT;
			}
			cbfr = 0xFF & (cbfr << 1);
			rdata_cnt++;
		}
		cbfr = (*(frame+cnt)).rgb.r;
		for(uint8_t c=0;c<8;c++)
		{
			if((cbfr & 0x80) == 0x80)
			{
				*(rawdata_ptr + rdata_cnt) = WS2818_LONG_CNT;
			}
			else
			{
				*(rawdata_ptr + rdata_cnt) = WS2818_SHORT_CNT;
			}
			cbfr = 0xFF & (cbfr << 1);
			rdata_cnt++;
		}
		cbfr = (*(frame+cnt)).rgb.b;
		for(uint8_t c=0;c<8;c++)
		{
			if((cbfr & 0x80) == 0x80)
			{
				*(rawdata_ptr + rdata_cnt) = WS2818_LONG_CNT;
			}
			else
			{
				*(rawdata_ptr + rdata_cnt) = WS2818_SHORT_CNT;
			}
			cbfr = 0xFF & (cbfr << 1);
			rdata_cnt++;
		}
	}
}

void DMA1_CH2_IRQHandler()
{

	// enable update interrupt, disable update dma request
	TIM2->DIER |= 1;
	TIM2->DIER &= ~(1 << UDE);

	// globally clear the interrupt flags
	DMA->IFCR = 1 << 4;

	// disable dma
    DMA->CHANNEL[1].CCR &= ~(1 << EN);
	return;
}

void initTimer()
{
	uint8_t dummy;
    RCC->APB1ENR|= 1 << TIM2EN; // enable timer 2
    RCC->AHBENR |= (1 << IOPAEN) | (1 << DMA1EN); // enable gpio a and dma 1

    GPIOA->MODER |= AF << PINA0POS; // alternate function in pina0
	GPIOA->AFRL |= 1 << PINA0POS; // AF1 of pina0 is TIM2_CH1

	// enable capture / compare 1
	TIM2->CCER |= 1;

    // DMA configuration

    //set peripheral register to capture/compare register 1 of timer 2
    DMA->CHANNEL[1].CPAR = (uint32_t)&(TIM2->CCR1);

    // set memory address to the raw data pointer
    DMA->CHANNEL[1].CMAR = (uint32_t)rawdata_ptr;

    DMA->CHANNEL[1].CCR |= (3 << PL) | (0 << MSIZE) | (2 << PSIZE) | (1 << MINC) | (1 << DIR) | (1 << TCIE);

    *NVIC_ISER0 |= (1 << 12) | (1 << 28); // enable channel 2 interrupt and tim2 global interrupt

    TIM2->CCMR1 |= (6 << OC1M) | (1 << OC1FE) |  (1<< OC1PE);


}

void sendToLed()
{

	// set timing
	TIM2->ARR = WS2818_CNT;

	// set the compare register to a low value to start with zero
	TIM2->CCR1 = 0x0;

	TIM2->CNT=0;

	// enable capture / compare 1
	TIM2->CCER |= 1;

    // set number of bytes to transfer
    DMA->CHANNEL[1].CNDTR=N_LAMPS*24+1;

	// enable dma request on update of channel 1, disable update interrupt
	TIM2->DIER |= (1 << UDE);
	TIM2->DIER &= ~1;

    DMA->CHANNEL[1].CCR |= (1 << EN);

	// enable timer
    TIM2->CR1 |= 1 << CEN;

    //generate update
    TIM2->EGR |= 1;



}
