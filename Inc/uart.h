/*
 * uart.h
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */
#ifdef STM32

#ifndef UART_H_
#define UART_H_

#include "types.h"

#define BAUD_RATE_USB 57600
#define BAUD_RATE_BT 9600

typedef struct
{
	reg CR1;
	reg CR2;
	reg CR3;
	reg BRR;
	reg GTPR;
	reg RTOR;
	reg RQR;
	reg ISR;
	reg ICR;
	reg RDR;
	reg TDR;
} UartTypeDef;

#define INPUT_BUFFER_SIZE 8
#define OUTPUT_BUFFER_SIZE 256

typedef struct
{
	char inputBuffer[INPUT_BUFFER_SIZE];
	char outputBuffer[OUTPUT_BUFFER_SIZE];
	uint32_t outputBufferReadCnt;
	uint32_t outputBufferWriteCnt;
	uint8_t inputBufferCnt;
} CommBufferType;

typedef CommBufferType* CommBuffer;

#define TXE (7)
#define TC (6)
#define RXNE (5)

#define UART1 ((UartTypeDef*)0x40013800UL)
#define UART2 ((UartTypeDef*)0x40004400UL)


void initUart();
void printf(const char*);

uint8_t sendCharAsyncUsb();
uint8_t sendCharAsyncBt();

void initBTUart();


#endif /* UART_H_ */

#endif
