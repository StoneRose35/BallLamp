/*
 * led_timer.c
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */
#include "led_timer.h"




void decompressRgbArray(RGBStream * frame,uint8_t length)
{
	uint8_t cnt = 0;
	uint8_t cbfr;
	uint16_t rdata_cnt = 0;
	for(cnt=0;cnt<length;cnt++)
	{
		// refer to specific hardware implementation to check which color comes first
		cbfr = (*(frame+cnt)).rgb.g;
		for(uint8_t c=0;c<8;c++)
		{
			if((cbfr & 0x80) == 0x80)
			{
				*(rawdata_ptr + (rdata_cnt << 2)) = WS2818_LONG_CNT;
			}
			else
			{
				*(rawdata_ptr + (rdata_cnt << 2)) = WS2818_SHORT_CNT;
			}
			cbfr = cbfr << 1;
			rdata_cnt++;
		}
		cbfr = (*(frame+cnt)).rgb.r;
		for(uint8_t c=0;c<8;c++)
		{
			if((cbfr & 0x80) == 0x80)
			{
				*(rawdata_ptr + (rdata_cnt << 2)) = WS2818_LONG_CNT;
			}
			else
			{
				*(rawdata_ptr + (rdata_cnt << 2)) = WS2818_SHORT_CNT;
			}
			cbfr = cbfr << 1;
			rdata_cnt++;
		}
		cbfr = (*(frame+cnt)).rgb.b;
		for(uint8_t c=0;c<8;c++)
		{
			if((cbfr & 0x80) == 0x80)
			{
				*(rawdata_ptr + (rdata_cnt << 2)) = WS2818_LONG_CNT;
			}
			else
			{
				*(rawdata_ptr + (rdata_cnt << 2)) = WS2818_SHORT_CNT;
			}
			cbfr = cbfr << 1;
			rdata_cnt++;
		}
	}
}

void DMA1_Stream1_IRQHandler()
{
	// all data transferred, disable timer
	TIM2->CR1 &= ~(1 << CEN);

	// globally clear the interrupt flags
	DMA->IFCR = 1 << 4;
	return;
}

void initTimer()
{

    *RCC_APB1ENR |= 1 << TIM2EN;

    TIM2->CCMR1 |= (6 << OC1M) | (1 << OC1PE) | (1 << OC1FE); // set PWM settings

    GPIOA->MODER |= AF << PINA0POS; // alternate function in pina0
	GPIOA->AFRL |= 1 << PINA0POS; // AF1 of pina0 is TIM2_CH1

	// set timing
	TIM2->ARR = WS2818_CNT;

	// enable dma request on CCR1
	TIM2->DIER |= 1 << UDE;

    // DMA configuration

    //set peripheral register to capture/compare register 1 of timer 2
    DMA->CHANNEL[1].CPAR = (uint32_t)&(TIM2->CCR1);

    // set memory address to the raw data pointer
    DMA->CHANNEL[1].CMAR = (uint32_t)rawdata_ptr;

    // set number of bytes to transfer
    DMA->CHANNEL[1].CNDTR=80*24;

    DMA->CHANNEL[1].CCR |= (3 << PL) | (2 << MSIZE) | (2 << PSIZE) | (1 << MINC) | (1 << DIR) | (1 << TCIE);
    DMA->CHANNEL[1].CCR |= (1 << EN);

}

void sendToLed()
{
    // set timer value to generate an update dma request shortly
    TIM2->CNT = WS2818_CNT - 1;
	// enable timer
    TIM2->CR1 |= 1 << CEN;
}
