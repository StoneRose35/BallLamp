/*
 * led_timer.c
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */
#ifdef STM32

#include "neopixelDriver.h"
volatile uint8_t sendState=SEND_STATE_INITIAL;

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
			if (sendState != SEND_STATE_DATA_READY)
			{
				sendState = SEND_STATE_BUFFER_UNDERRUN;
			}
			else
			{
				sendState = SEND_STATE_RTS;
			}
		}
		else
		{
			// finished clocking of the last data bit
			TIM2->ARR = REMAINING_WAITTIME; //1979733;
			TIM2->CCR3 = 0x0;
            sendState = SEND_STATE_SENT;
		}

		TIM2->SR &= ~(0x1); // clear interrupt
	}
}
#endif

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

void I2C1_EV_EXTI23_IRQHandler()
{

}

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
	sendState = SEND_STATE_DATA_READY;
}

/*
 * Initiates the GPIO, TIM2 and DMA necessary for operating the neopixel array
 * should be called once after startup
 */
void initTimer()
{
    RCC->APB1ENR|= 1 << TIM2EN; // enable timer 2
    RCC->AHBENR |= (1 << IOPAEN) | (1 << DMA1EN); // enable gpio a and dma 1

    GPIOA->MODER |= AF << 18; // alternate function in pina9
	GPIOA->AFRH |= 10 << 4; //AF10 of pina9 is TIM2_CH3

	GPIOA->OTYPER |= (1 << 9);


	// enable capture / compare 1
	TIM2->CCER |= (1 << 8); // for ccr channel 3;

    // DMA configuration

    //set peripheral register to capture/compare register 1 of timer 2
	DMA->CHANNEL[1].CPAR = (uint32_t)&(TIM2->CCR3);

    // set memory address to the raw data pointer
    DMA->CHANNEL[1].CMAR = (uint32_t)rawdata_ptr;

    DMA->CHANNEL[1].CCR |= (3 << PL) | (0 << MSIZE) | (2 << PSIZE) | (1 << MINC) | (1 << DIR) | (1 << TCIE);

    *NVIC_ISER0 |= (1 << 12) | (1 << 28); // enable channel 2 interrupt and tim2 global interrupt

    TIM2->CCMR2 |= (6 << OC1M) | (1 << OC1FE) |  (1 << OC1PE); // settings for CCR channel 3

}

/* non-blocking function which initiates a data transfer to the neopixel array
 * be aware that rawdata should not be modified as long as tim2 is in neopixel clocking mode
 * (counts up to WS2818_CNT)
 * */
void sendToLed()
{

	sendState = SEND_STATE_SENDING;
	// set timing
	TIM2->ARR = WS2818_CNT;

	// set the compare register to a low value to start with zero
	TIM2->CCR3 = 0x0;

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

uint8_t getSendState()
{
	return sendState;
}

void setSendState(uint8_t s)
{
	sendState = s;
}

void i2cInit()
{
	RCC->APB1ENR |= (1 << I2C1EN);

	I2C1->TIMINGR = 0x00101D2D; // taken from the handy configurator: standard mode without analog filter

	// enable gpio b
	RCC->AHBENR |= (1 << IOPBEN);

	// alternate function 4 for pb6 und pb7
    GPIOB->AFRL |= (4 << 24) | (4 << 28);

	// open drain from pb6 and pb7
	GPIOB->OTYPER |= (1 << 6) | (1 << 7);

	// enable i2c1 event interrupt
	*NVIC_ISER0 |= (1 << 31);

	//set address
	I2C1->OAR1 |= (I2C_ADDRESS << 1) | (1 << OA1EN);

	I2C1->CR1 |= (1 << 2) | (1 << 0); // enable i2c1

}

#endif
