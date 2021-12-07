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

/*save a stream "data" of length "size" at the beginning of the "filesystem" flash section
 * offset denotes the offset from the start in bytes, data from the filesystem start to the offset is retained while writing
 * */
uint16_t saveData(uint16_t * data,uint32_t size,uint32_t offset)
{
	uint16_t retcode=0;
	uint8_t sizeInPages = (uint8_t)(((size+offset) >> 11) + 1); // /2k
	uint16_t* flashHeader=(uint16_t*)malloc(offset);

	for(uint8_t c=0;c<offset>>1;c++)
	{
		*(flashHeader+c)=*((uint16_t*)(getFilesystemStart())+c);
	}
	for (uint8_t c=0;c<sizeInPages;c++)
	{
		retcode += erasePage(c);
	}

	for(uint8_t c=0;c<offset;c+=2)
	{
		retcode += programHalfword(*(flashHeader+(c>>1)),getFilesystemStart() + c);
	}
	for (uint32_t c2=0;c2<size;c2+=2)
	{
		retcode += programHalfword(*(data+(c2>>1)),getFilesystemStart()+c2+offset);
	}
	free(flashHeader);
	return retcode;
}
