#ifndef STM32

#include "flash.h"
#include "fakeFlash.h"


uint16_t fakeflash[1024*1024];

uint32_t __filesystem_start;

void unlockFlash()
{

}

uint8_t erasePage(uint8_t pagenr)
{
	return 0;
}
uint8_t programHalfword(uint16_t hwrd,uint32_t addr)
{
	return 0;
}

uint16_t saveInFlash(uint16_t * data,uint32_t size,uint32_t offset)
{
	for(uint32_t c=0;c<size;c+=2)
	{
		*(fakeflash+c) = *(data+c);
	}
	return 0;
}

#endif
