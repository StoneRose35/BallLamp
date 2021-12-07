/*
 * uart.h
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */
#include "systemChoice.h"
#include "types.h"

#ifdef HARDWARE
#ifndef UART_H_
#define UART_H_



#define BAUD_RATE_USB 57600
#define BAUD_RATE_BT 9600

#ifdef STM32
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

#define TXE (7)
#define TC (6)
#define RXNE (5)

#define UART1 ((UartTypeDef*)0x40013800UL)
#define UART2 ((UartTypeDef*)0x40004400UL)
#endif

#ifdef RP2040_FEATHER
#endif


void initUart();
void printf(const char*);

uint8_t sendCharAsyncUsb();
uint8_t sendCharAsyncBt();

void initBTUart();


#endif /* UART_H_ */

#endif
