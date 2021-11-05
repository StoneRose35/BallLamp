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

extern uint8_t __filesystem_start;

uint16_t saveHeader(uint16_t * data,uint32_t size)
{
	uint16_t retcode=0;
	uint32_t pageBfr[FLASH_PAGE_SIZE >> 2];
	for(uint16_t c=0;c<FLASH_PAGE_SIZE; c+=4)
	{
		pageBfr[c>>2] = *(&__filesystem_start + c);
	}

	erasePage(1);

	for (uint32_t c2=0;c2<size;c2+=2)
	{
		retcode += programHalfword(*(data+(c2>>1)),(ptr)&__filesystem_start+c2);
	}
	for (uint32_t c3=0;c3<(FLASH_PAGE_SIZE-size);c3+=2)
	{
		retcode += programHalfword(*((uint16_t*)(pageBfr+((c3>>1) + size))),(ptr)&__filesystem_start+size+c3);
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

	for(uint8_t c=0;c<offset;c+=2)
	{
		*(flashHeader+(c>>1))=*(&__filesystem_start+c);
	}
	for (uint8_t c=0;c<sizeInPages;c++)
	{
		retcode += erasePage(c);
	}

	for(uint8_t c=0;c<offset;c+=2)
	{
		retcode += programHalfword(*(flashHeader+(c>>1)),(ptr)&__filesystem_start + c);
	}
	for (uint32_t c2=0;c2<size;c2+=2)
	{
		retcode += programHalfword(*(data+(c2>>1)),(ptr)&__filesystem_start+c2+offset);
	}
	free(flashHeader);
	return retcode;
}
