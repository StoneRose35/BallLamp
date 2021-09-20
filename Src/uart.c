/*
 * uart.c
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */

#include <neopixelDriver.h>
#include "uart.h"
#include "system.h"
#include "consoleHandler.h"

volatile char inputBuffer[8];
volatile uint8_t inputBufferCnt=0;
extern uint32_t task;

void USART2_EXTI26_IRQHandler()
{
	if ((UART2->ISR & (1 << RXNE)) == (1 << RXNE)) // reception case
	{
		inputBuffer[inputBufferCnt++]=UART2->RDR & 0xFF;
		inputBufferCnt &= 0x7;
		task |= (1 << TASK_CONSOLE);
	}

}

/*send a character to the uart, this calls blocks as long as a transmission is ongoing!*/
void sendChar(uint8_t c)
{
	while ((UART2->ISR & (1 << TC)) == 0);
	UART2->TDR = c;
}

void printf(const char* data)
{
	uint32_t cnt = 0;
	uint8_t cur_data;
	cur_data = *(data + cnt);
	while (cur_data != 0)
	{
		sendChar(cur_data);
		cnt++;
		cur_data = *(data + cnt);
	}
}

void initUart()
{
	// GPIO Settings
	RCC->AHBENR |= (1 << IOPAEN);
	GPIOA->MODER |= (AF << 4) | (AF << 30); // alternate function for pa2 and pa15
	GPIOA->AFRL |= (7 << 8); // PA2: UART2.TX
	GPIOA->AFRH |= (7 << 28); //PA15: UART2.RX


	// enable uart2 in rcc
	RCC->APB1ENR |= (1 << 17);

    // CR1 enable bits 5,3,2 Receiver not empty interrupt, Receiver and Transmitter on
	UART2->CR1 |= (1 << 5) | (1 << 3) | (1 << 2);

	// set baud rate
	UART2->BRR = F_APB1/BAUD_RATE;

	// enable uart2 interrupt, at pos 38 --> pos 6 of 2nd register
	*NVIC_ISER1 |= (1 << 6);

	UART2->CR1 |= 1;  //uart enable
}
