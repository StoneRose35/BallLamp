/*
 * uart.c
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */

#ifdef STM32

#include <neopixelDriver.h>
#include "uart.h"
#include "system.h"
#include "consoleHandler.h"


extern uint32_t task;
extern uint8_t context; // used by printf to decide where a certain information should be output

CommBufferType usbCommBuffer;
CommBufferType btCommBuffer;

void USART1_EXTI25_IRQHandler()
{
	if ((UART1->ISR & (1 << RXNE)) == (1 << RXNE)) // reception case
	{
		btCommBuffer.inputBuffer[btCommBuffer.inputBufferCnt++]=UART1->RDR & 0xFF;
		btCommBuffer.inputBufferCnt &= (INPUT_BUFFER_SIZE-1);
		task |= (1 << TASK_BT_CONSOLE);
	}
}

void USART2_EXTI26_IRQHandler()
{
	if ((UART2->ISR & (1 << RXNE)) == (1 << RXNE)) // reception case
	{
		usbCommBuffer.inputBuffer[usbCommBuffer.inputBufferCnt++]=UART2->RDR & 0xFF;
		usbCommBuffer.inputBufferCnt &= (INPUT_BUFFER_SIZE-1);
		task |= (1 << TASK_USB_CONSOLE);
	}

}

uint8_t sendCharAsyncUsb()
{
	if (usbCommBuffer.outputBufferWriteCnt < usbCommBuffer.outputBufferReadCnt && ((UART2->ISR & (1 << TC)) == (1 << TC)))
	{
		UART2->TDR = *(usbCommBuffer.outputBuffer+usbCommBuffer.outputBufferWriteCnt);
		usbCommBuffer.outputBufferWriteCnt++;
	}
	if (usbCommBuffer.outputBufferWriteCnt == usbCommBuffer.outputBufferReadCnt)
	{
		usbCommBuffer.outputBufferWriteCnt=0;
		usbCommBuffer.outputBufferReadCnt=0;
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t sendCharAsyncBt()
{
	if (btCommBuffer.outputBufferWriteCnt < btCommBuffer.outputBufferReadCnt && ((UART1->ISR & (1 << TC)) == (1 << TC)))
	{
		UART1->TDR = *(btCommBuffer.outputBuffer+btCommBuffer.outputBufferWriteCnt);
		btCommBuffer.outputBufferWriteCnt++;
	}
	if (btCommBuffer.outputBufferWriteCnt == btCommBuffer.outputBufferReadCnt)
	{
		btCommBuffer.outputBufferWriteCnt=0;
		btCommBuffer.outputBufferReadCnt=0;
		return 1;
	}
	else
	{
		return 0;
	}
}

void printf(const char* data)
{
	uint32_t cnt = 0;
	uint8_t cur_data;
	cur_data = *(data + cnt);
	while (cur_data != 0)
	{
		if ((context & (1 << CONTEXT_USB)) == (1 << CONTEXT_USB))
		{
			*(usbCommBuffer.outputBuffer+usbCommBuffer.outputBufferReadCnt) = *(data + cnt);
			usbCommBuffer.outputBufferReadCnt++;
			usbCommBuffer.outputBufferReadCnt &= (OUTPUT_BUFFER_SIZE-1);

			if (usbCommBuffer.outputBufferReadCnt==(OUTPUT_BUFFER_SIZE-1))
			{
				uint8_t sc_res = sendCharAsyncUsb();
				while (sc_res == 0)
				{
					sc_res = sendCharAsyncUsb();
				}
			}

		}
		if ((context & (1 << CONTEXT_BT)) == (1 << CONTEXT_BT))
		{
			*(btCommBuffer.outputBuffer+btCommBuffer.outputBufferReadCnt) = *(data + cnt);
			btCommBuffer.outputBufferReadCnt++;
			btCommBuffer.outputBufferReadCnt &= (OUTPUT_BUFFER_SIZE-1);

			if (btCommBuffer.outputBufferReadCnt==(OUTPUT_BUFFER_SIZE-1))
			{
				uint8_t sc_res = sendCharAsyncBt();
				while (sc_res == 0)
				{
					sc_res = sendCharAsyncBt();
				}
			}
		}
		cnt++;
		cur_data = *(data + cnt);
	}
}

/* USB Uart, used for serial communication over usb
 * hooked up to pa2 (TX) and pa15 (rx)
 * */
void initUart()
{
	// GPIO Settings
	RCC->AHBENR |= (1 << IOPAEN);
	GPIOA->MODER |= (AF << 4) | (AF << 30); // alternate function for pa2 and pa15
	GPIOA->AFRL |= (7 << 2*4); // PA2: UART2.TX
	GPIOA->AFRH |= (7 << 7*4); //PA15: UART2.RX


	// enable uart2 in rcc
	RCC->APB1ENR |= (1 << 17);

    // CR1 enable bits 5,3,2 Receiver not empty interrupt, Receiver and Transmitter on
	UART2->CR1 |= (1 << 5) | (1 << 3) | (1 << 2);

	// set baud rate
	UART2->BRR = F_APB1/BAUD_RATE_USB;

	// enable uart2 interrupt, at pos 38 --> pos 6 of 2nd register
	*NVIC_ISER1 |= (1 << 6);

	UART2->CR1 |= 1;  //uart enable
}

/*
 * Uart connected to the bluetooth module
 * pb6 is tx and pb7 is rx
 * */
void initBTUart()
{
	// GPIO Settings
	RCC->AHBENR |= (1 << IOPBEN);
	GPIOB->MODER |= (AF << 6*2) | (AF << 7*2); // alternate function for pb6 and pb7
	GPIOB->AFRL |= (7 << 6*4) | (7 << 7*4); // PB6: UART2.TX, PB7: UART2.RX
	GPIOB->PUPDR |= (1 << 7*2); // enable pullup on receiver side

	// enable uart1 in rcc
	RCC->APB2ENR |= (1 << 14);

    // CR1 enable bits 5,3,2 Receiver not empty interrupt, Receiver and Transmitter on
	UART1->CR1 |= (1 << 5) | (1 << 3) | (1 << 2);

	// set baud rate
	UART1->BRR = F_APB1/BAUD_RATE_BT;

	// enable uart1 interrupt, at pos 37 --> pos 5 of 2nd register
	*NVIC_ISER1 |= (1 << 5);

	UART1->CR1 |= 1;  //uart enable
}

#endif
