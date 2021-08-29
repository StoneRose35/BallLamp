/*
 * uart.h
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */

#ifndef UART_H_
#define UART_H_

#include "types.h"

#define BAUD_RATE 9600
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

void initUart();
void printf(char*);
void sendChar(uint8_t);


#endif /* UART_H_ */