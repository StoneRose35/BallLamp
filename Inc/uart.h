/*
 * uart.h
 *
 *  Created on: 29.08.2021
 *      Author: philipp
 */
#include "systemChoice.h"
#include <stdint.h>

#ifdef HARDWARE
#ifndef UART_H_
#define UART_H_



#define BAUD_RATE 57600

#ifdef STM32
typedef volatile uint32_t reg;
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


void initUart(uint16_t baudrate);
void initBTUart(uint16_t baudrate);
void initGpio();

void printf(const char*);

uint8_t sendCharAsyncUsb();
uint8_t sendCharAsyncBt();




#endif /* UART_H_ */

#endif
