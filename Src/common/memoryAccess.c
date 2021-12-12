/*
 * memoryAccess.c
 *
 *  Created on: 05.11.2021
 *      Author: philipp
 */


#include "types.h"

#include <stdlib.h>
#include "flash.h"
#include "memoryAccess.h"


uint16_t saveHeader(uint16_t * data,uint32_t size)
{
	uint16_t retcode=0;
	uint32_t pageBfr[FLASH_PAGE_SIZE >> 2];
	for(uint16_t c=0;c<FLASH_PAGE_SIZE>>2; c++)
	{
		pageBfr[c] = *((uint32_t*)(getFilesystemStart() + c));
	}

	erasePage(1);

	for (uint32_t c2=0;c2<size;c2+=2)
	{
		retcode += programHalfword(*(data+(c2>>1)),getFilesystemStart()+c2);
	}
	for (uint32_t c3=0;c3<(FLASH_PAGE_SIZE-size);c3+=2)
	{
		retcode += programHalfword(*((uint16_t*)(pageBfr+((c3>>1) + size))),getFilesystemStart()+size+c3);
	}
	return retcode;
}

uint16_t saveData(uint16_t * data,uint32_t size,uint32_t offset)
{
	uint16_t retcode=0;
	uint8_t sizeInPages = (uint8_t)((size+offset) >> FLASH_PAGE_SIZE_BIT); 
	uint8_t firstPage = (uint8_t)(offset >> FLASH_PAGE_SIZE_BIT);
	uint32_t dataCnt=0;
	uint32_t flashCnt=0;
	uint32_t pageOffset=0;
	uint16_t pageBuffer[FLASH_PAGE_SIZE>>2];

	ptr fsStart = getFilesystemStart();

	for (uint8_t c=0;c<sizeInPages;c++)
	{
		if(c==0)
		{
			pageOffset = (offset - ((offset >> FLASH_PAGE_SIZE_BIT)<<FLASH_PAGE_SIZE_BIT))>>FLASH_PAGE_SIZE_BIT; 
		}
		else
		{
			pageOffset=0;
		}
		for (uint16_t c2=0;c2<FLASH_PAGE_SIZE>>1;c2++)
		{
			if(dataCnt < size && c2 > pageOffset)
			{
				pageBuffer[c2] = *(data+dataCnt++);
			}
			else
			{
				pageBuffer[c2] = *((uint16_t*)fsStart + flashCnt++);
			}
		}
		erasePage(c);
		programPage(c,pageBuffer,FLASH_PAGE_SIZE>>1);
	}

	return 0;
}