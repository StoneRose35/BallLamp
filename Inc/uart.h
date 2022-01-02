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
#define UART_USB_RX 1
#define UART_USB_TX 0

#define UART_BT_RX 9
#define UART_BT_TX 8

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

#define UART_UARTIBRD ((volatile uint32_t*)(UART0_BASE+UART_UARTIBRD_OFFSET))
#define UART_UARTFBRD ((volatile uint32_t*)(UART0_BASE+UART_UARTFBRD_OFFSET))
#define UART_UARTCR ((volatile uint32_t*)(UART0_BASE+UART_UARTCR_OFFSET))
#define UART_UARTLCR_H ((volatile uint32_t*)(UART0_BASE+UART_UARTLCR_H_OFFSET))
#define UART_RX_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*UART_USB_RX))
#define UART_TX_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*UART_USB_TX))
#define UART_UARTIMSC ((volatile uint32_t*)(UART0_BASE+UART_UARTIMSC_OFFSET))
#define UART_UARTMIS ((volatile uint32_t*)(UART0_BASE+UART_UARTMIS_OFFSET))
#define UART_UARTCR ((volatile uint32_t*)(UART0_BASE+UART_UARTCR_OFFSET))
#define UART_UARTDR ((volatile uint32_t*)(UART0_BASE+UART_UARTDR_OFFSET))
#define UART_UARTRIS ((volatile uint32_t*)(UART0_BASE+UART_UARTRIS_OFFSET))
#define UART_UARTFR ((volatile uint32_t*)(UART0_BASE+UART_UARTFR_OFFSET))

#define UARTBT_UARTIBRD ((volatile uint32_t*)(UART1_BASE+UART_UARTIBRD_OFFSET))
#define UARTBT_UARTFBRD ((volatile uint32_t*)(UART1_BASE+UART_UARTFBRD_OFFSET))
#define UARTBT_UARTCR ((volatile uint32_t*)(UART1_BASE+UART_UARTCR_OFFSET))
#define UARTBT_UARTLCR_H ((volatile uint32_t*)(UART1_BASE+UART_UARTLCR_H_OFFSET))
#define UARTBT_RX_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*UART_BT_RX))
#define UARTBT_TX_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*UART_BT_TX))
#define UARTBT_UARTIMSC ((volatile uint32_t*)(UART1_BASE+UART_UARTIMSC_OFFSET))
#define UARTBT_UARTMIS ((volatile uint32_t*)(UART1_BASE+UART_UARTMIS_OFFSET))
#define UARTBT_UARTCR ((volatile uint32_t*)(UART1_BASE+UART_UARTCR_OFFSET))
#define UARTBT_UARTDR ((volatile uint32_t*)(UART1_BASE+UART_UARTDR_OFFSET))
#define UARTBT_UARTRIS ((volatile uint32_t*)(UART1_BASE+UART_UARTRIS_OFFSET))
#define UARTBT_UARTFR ((volatile uint32_t*)(UART1_BASE+UART_UARTFR_OFFSET))


void initUart(uint16_t baudrate);
void initBTUart(uint16_t baudrate);
void initGpio();

void printf(const char*);

uint8_t sendCharAsyncUsb();
uint8_t sendCharAsyncBt();




#endif /* UART_H_ */

#endif
