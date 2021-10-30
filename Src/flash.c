/*
 * flash.c
 *
 *  Created on: 27.10.2021
 *      Author: philipp
 */

#ifdef STM32

#include "types.h"
#include "neopixelDriver.h"

uint32_t pageBuffer[2048/4];
extern uint32_t __filesystem_start;

void unlockFlash()
{
	FLASH->KEYR = 0x45670123L;
	FLASH->KEYR = 0xCDEF89AB;
}
uint8_t erasePage(uint8_t pagenr)
{
	if ((FLASH->CR & (1 << FLASH_LOCK))==(1 << FLASH_LOCK))
	{
		unlockFlash();
	}

	while((FLASH->SR & (1 << FLASH_BSY))==(1 << FLASH_BSY));
	FLASH->CR |= (1 << FLASH_PER);
	FLASH->AR = __filesystem_start/0x800 + pagenr;
	FLASH->CR|= (1 << FLASH_STRT);
	while((FLASH->SR & (1 << FLASH_BSY))==(1 << FLASH_BSY));
	if ((FLASH->SR & (1 << FLASH_EOP))== (1 << FLASH_EOP))
	{
		FLASH->SR &= ~(1 << FLASH_EOP);
		return 0;
	}
	return 1;
}

uint8_t programHalfword(uint16_t hwrd,uint32_t addr)
{
	uint16_t readBack;
	if ((FLASH->CR & (1 << FLASH_LOCK))==(1 << FLASH_LOCK))
	{
		unlockFlash();
	}

	FLASH->CR |= (1 << FLASH_PG);
	*((uint16_t*)addr)=hwrd;
	while((FLASH->SR & (1 << FLASH_BSY))==(1 << FLASH_BSY));

	readBack = *((uint16_t*)addr);
	if (readBack == hwrd)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

/*save a stream "data" of length "size" at the beginning of the "filesystem" flash section*/
uint16_t saveInFlash(uint16_t * data,uint32_t size,uint32_t offset)
{
	uint16_t retcode=0;
	uint8_t sizeInPages = (uint8_t)((size >> 11) + 1); // /2k
	uint8_t initialPage = __filesystem_start >> 11;
	for (uint8_t c=0;c<sizeInPages;c++)
	{
		retcode += erasePage(c+initialPage);
	}
	for (uint32_t c2=0;c2<size;c2+=2)
	{
		retcode += programHalfword(*(data+(c2>>1)),__filesystem_start+c2+offset);
	}
	return retcode;
}

#endif
