#ifndef STM32

#include "flash.h"


void unlockFlash()
{

}

uint8_t erasePage(uint8_t pagenr)
{
	for(uint16_t c=(pagenr-1)<<10;c<((pagenr-1)<<10) + (FLASH_PAGE_SIZE>>1);c++)
	{
		*(fakeflash +c)=0xFFFF;
	}
	return 0;
}
uint8_t programHalfword(uint16_t hwrd,ptr addr)
{
	*((uint16_t*)addr)=hwrd;
	return 0;
}


#endif
