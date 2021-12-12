/*
 * uart.c
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */

#ifdef RP2040_FEATHER

#include <neopixelDriver.h>
#include "uart.h"
#include "system.h"
#include "consoleHandler.h"
#include "bufferedInputHandler.h"

#include "hardware/regs/addressmap.h"
#include"hardware/regs/uart.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/m0plus.h"


extern uint32_t task;
extern uint8_t context; // used by printf to decide where a certain information should be output

CommBufferType usbCommBuffer;
CommBufferType btCommBuffer;

// receive interrupt for the usb uart
void isr_uart0_irq20()
{
	if ((*UART_UARTCR & (1 << UART_UARTICR_RXIC_LSB)) == (1 << UART_UARTICR_RXIC_LSB)) // reception case
	{
		usbCommBuffer.inputBuffer[usbCommBuffer.inputBufferCnt++]=*UART_UARTDR & 0xFF;
		usbCommBuffer.inputBufferCnt &= (INPUT_BUFFER_SIZE-1);
		task |= (1 << TASK_USB_CONSOLE);
		//*UART_UARTCR |= (1 << UART_UARTICR_RXIC_LSB); // clear interrupt flag
	}
}

// receive interrupt for the bluetooth uart
void isr_uart1_irq21()
{
	if ((*UARTBT_UARTCR & (1 << UART_UARTICR_RXIC_LSB)) == (1 << UART_UARTICR_RXIC_LSB)) // reception case
	{
		btCommBuffer.inputBuffer[btCommBuffer.inputBufferCnt++]=*UARTBT_UARTDR & 0xFF;
		btCommBuffer.inputBufferCnt &= (INPUT_BUFFER_SIZE-1);
		task |= (1 << TASK_BT_CONSOLE);
		//*UARTBT_UARTCR |= (1 << UART_UARTICR_RXIC_LSB); // clear interrupt flag
	}
}

uint8_t sendCharAsyncUsb()
{
	if (usbCommBuffer.outputBufferWriteCnt < usbCommBuffer.outputBufferReadCnt && ((*UART_UARTRIS & (1 << UART_UARTRIS_TXRIS_LSB)) == (1 << UART_UARTRIS_TXRIS_LSB)))
	{
		*UART_UARTDR = *(usbCommBuffer.outputBuffer+usbCommBuffer.outputBufferWriteCnt);
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
	if (btCommBuffer.outputBufferWriteCnt < btCommBuffer.outputBufferReadCnt && ((*UARTBT_UARTRIS & (1 << UART_UARTRIS_TXRIS_LSB)) == (1 << UART_UARTRIS_TXRIS_LSB)))
	{
		*UARTBT_UARTDR = *(btCommBuffer.outputBuffer+btCommBuffer.outputBufferWriteCnt);
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
 * 
 * */
void initUart()
{
	// power on uart
    *RESETS &= ~(1 << RESETS_RESET_UART0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_UART0_LSB)) == 0);

	// wire up the GPIO to UART
	*UART_RX_PIN_CNTR = 2;
	*UART_TX_PIN_CNTR = 2;

	// enable interrupt on receive
	*UART_UARTIMSC |= (1 << UART_UARTIMSC_RXIM_LSB);

	// clock is 132MHz / (16*57600)
	// resulting in 143.2291666, becoming 143 and floor(0.2291666*64)=14
	*UART_UARTIBRD = 143;
	*UART_UARTFBRD = 14;

	// set word length to 8 bits
	*UART_UARTLCR_H |= (3 << UART_UARTLCR_H_WLEN_LSB);

	// enable receive and transmit
	*UART_UARTCR |= (1 << UART_UARTCR_RXE_LSB) | (1 << UART_UARTCR_TXE_LSB) | (1 << UART_UARTCR_UARTEN_LSB);

	// enable interrupts in the nvic
	*NVIC_ISER = (1 << 20);

}

/*
 * Uart connected to the bluetooth module
 * 
 * */
void initBTUart()
{
	// power on uart
    *RESETS &= ~(1 << RESETS_RESET_UART1_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_UART1_LSB)) == 0);

	// wire up the GPIO to UART
	*UARTBT_RX_PIN_CNTR = 2;
	*UARTBT_TX_PIN_CNTR = 2;

	// enable interrupt on receive
	*UARTBT_UARTIMSC |= (1 << UART_UARTIMSC_RXIM_LSB);

	// clock is 132MHz / (16*57600)
	// resulting in 143.2291666, becoming 143 and floor(0.2291666*64)=14
	*UARTBT_UARTIBRD = 143;
	*UARTBT_UARTFBRD = 14;

	// set word length to 8 bits
	*UARTBT_UARTLCR_H |= (3 << UART_UARTLCR_H_WLEN_LSB);

	// enable receive and transmit
	*UARTBT_UARTCR |= (1 << UART_UARTCR_RXE_LSB) | (1 << UART_UARTCR_TXE_LSB) | (1 << UART_UARTCR_UARTEN_LSB);

	// enable interrupts in the nvic
	*NVIC_ISER = (1 << 21);
}

#endif
