/*
 * bluetoothATConfig.c
 *
 *  Created on: 04.11.2021
 *      Author: philipp
 */

#ifdef STM32
#include "types.h"
#include "uart.h"
#include "neopixelDriver.h"
#include "stringFunctions.h"
#include "memoryAccess.h"

const char * ATCommandCheck = "AT";
const char * ATCommandSetNamePrefix = "AT+NAME";
const char * ATCommandSetBaudPrefix = "AT+BAUD";
const char * ATCommandSetPinPrefix = "AT+PIN";

uint8_t ATSendCommand(char * cmd,char * out)
{
	uint8_t cnt = 0;
	*NVIC_ICER1 |= (1 << 5); // clear interrupt since config is done synchronously

	while (cmd[cnt] != 0)
	{
		while (!((UART2->ISR & (1 << TC)) == (1 << TC)));
		UART1->TDR =cmd[cnt++];
	}

	cnt=0;

	// enable timer3 as as timeout watcher

	RCC->APB1ENR|= 1 << TIM3EN; // enable timer 3
	TIM3->PSC = 0xFFFF;
	TIM3->CNT = 0;
	TIM3->CR1 |= 1;

	// wait ~1s for command (since no end sign is returned a timeout
	while (TIM3->CNT < 1000){
		if ((UART1->ISR & (1 << RXNE)) == (1 << RXNE))
		{
			out[cnt++]=UART1->RDR & 0xFF;
		}
	}

	TIM3->CR1 &= ~1;

	return cnt;
}

uint8_t ATCheckEnabled()
{
	char response[32];
	char request[32];
	uint8_t c=0,creq=0;
	while (ATCommandCheck[c] != 0)
	{
		request[creq]=ATCommandSetNamePrefix[c];
		c++;
		creq++;
	}
	ATSendCommand(request,response);
	if (startsWith(response,"OK") > 0)
	{
		return 0;
	}
	return 1;
}

uint8_t ATSetName(char * name)
{
	uint8_t c=0,creq=0;
	char response[32];
	char request[32];
	while (ATCommandSetNamePrefix[c] != 0)
	{
		request[creq]=ATCommandSetNamePrefix[c];
		c++;
		creq++;
	}
	c=0;
	while(name[c] != 0)
	{
		request[creq]=name[c];
		c++;
		creq++;
	}
	request[creq] = 0;


	ATSendCommand(request,response);
	if (startsWith(response,"OKsetname") > 0)
	{
		return 0;
	}
	return 1;
}

uint8_t ATSetBaud(char * idx)
{
	uint8_t c=0,creq=0;
	char response[32];
	char request[32];
	uint16_t baudrate[2];
	while (ATCommandSetNamePrefix[c] != 0)
	{
		request[creq]=ATCommandSetNamePrefix[c];
		c++;
		creq++;
	}
	request[creq++] = idx[0];
	request[creq] = 0;


	ATSendCommand(request,response);
	if (startsWith(response,"OK") > 0)
	{

		UART1->CR1 &= ~(1); // disable uart to be able reconfigure baud rate

		// change serial baud rate according to index
		switch((uint8_t)idx[0] - 0x30)
		{
		case 1:
			UART1->BRR = F_APB1/1200;
			break;
		case 2:
			UART1->BRR = F_APB1/2400;
			break;
		case 3:
			UART1->BRR = F_APB1/4800;
			break;
		case 4:
			UART1->BRR = F_APB1/9600;
			break;
		case 5:
			UART1->BRR = F_APB1/19200;
			break;
		case 6:
			UART1->BRR = F_APB1/38400;
			break;
		case 7:
			UART1->BRR = F_APB1/57600;
			break;
		case 8:
			UART1->BRR = F_APB1/115200;
			break;
		}

		// save actual bluetooth baud rate in position 0 in the flash header
		*baudrate=UART1->BRR;
		saveHeader(baudrate,2);

		UART1->CR1 |= (1);

		return 0;
	}
	return 1;
}

uint8_t ATSetPin(char * pin)
{
	uint8_t c=0,creq=0;
	char response[32];
	char request[32];
	while (ATCommandSetPinPrefix[c] != 0)
	{
		request[creq]=ATCommandSetPinPrefix[c];
		c++;
		creq++;
	}
	c=0;
	while(pin[c] != 0)
	{
		request[creq]=pin[c];
		c++;
		creq++;
	}
	request[creq] = 0;


	ATSendCommand(request,response);
	if (startsWith(response,"OK") > 0)
	{
		return 0;
	}
	return 1;
}

#endif
