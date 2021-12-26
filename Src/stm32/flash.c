/*
 * flash.c
 *
 *  Created on: 27.10.2021
 *      Author: philipp
 */

#ifdef STM32

#include <stdint.h>
#include "neopixelDriver.h"
#include "stdlib.h"
#include "flash.h"


void unlockFlash()
{
	FLASH->KEYR = 0x45670123L;
	FLASH->KEYR = 0xCDEF89AB;
}
uint8_t erasePage(uint16_t pagenr)
{
	if ((FLASH->CR & (1 << FLASH_LOCK))==(1 << FLASH_LOCK))
	{
		unlockFlash();
	}

	while((FLASH->SR & (1 << FLASH_BSY))==(1 << FLASH_BSY));
	FLASH->CR &= ~(1 << FLASH_PG);
	FLASH->CR |= (1 << FLASH_PER);
	FLASH->AR = (uint32_t)getFilesystemStart() + (pagenr << 11);
	FLASH->CR|= (1 << FLASH_STRT);
	while((FLASH->SR & (1 << FLASH_BSY))==(1 << FLASH_BSY));
	if ((FLASH->SR & (1 << FLASH_EOP))== (1 << FLASH_EOP))
	{
		FLASH->SR &= ~(1 << FLASH_EOP);
		return 0;
	}
	return 1;
}

uint8_t programHalfword(uint16_t hwrd,uint16_t* addr)
{
	uint16_t readBack;
	uint8_t errcode = 0;
	if ((FLASH->CR & (1 << FLASH_LOCK))==(1 << FLASH_LOCK))
	{
		unlockFlash();
	}
	FLASH->CR &= ~(1 << FLASH_PER);
	FLASH->CR |= (1 << FLASH_PG);
	*((uint16_t*)addr)=hwrd;
	while((FLASH->SR & (1 << FLASH_BSY))==(1 << FLASH_BSY));

	if ((FLASH->SR & (1 << FLASH_EOP))== (1 << FLASH_EOP))
	{
		FLASH->SR &= ~(1 << FLASH_EOP);
	}
	else
	{
		errcode |= 0x1;
	}
	readBack = *((uint16_t*)addr);
	if (readBack == hwrd)
	{
		return 0;
	}
	else
	{
		errcode |= 0x2;
	}
	return errcode;
}

uint8_t programPage(uint16_t pagenr,uint16_t* data,uint16_t cnt)
{
	uint8_t errcode = 0;
	ptr flashOffset = getFilesystemStart() + (pagenr << FLASH_PAGE_SIZE_BIT);
	for(uint16_t c=0;c<cnt;c++)
	{
		errcode = programHalfword(*(data+c),(uint16_t*)flashOffset + (c));
		if(errcode> 0)
		{
			return errcode;
		}
	}
	return errcode;
}



#endif
