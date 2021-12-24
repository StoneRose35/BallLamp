/*
 * flash.h
 *
 *  Created on: 30.10.2021
 *      Author: philipp
 */

#ifndef FLASH_H_
#define FLASH_H_
#include "systemChoice.h"
#include "system.h"

#ifdef HARDWARE
inline ptr getFilesystemStart()
{
	extern uint8_t __filesystem_start;
	return (ptr)&__filesystem_start;
}

#else
	uint16_t fakeflash[1024*1024];
	inline ptr getFilesystemStart()
	{
		return (ptr)fakeflash;
	}
#endif


void unlockFlash();
uint8_t erasePage(uint8_t pagenr);
uint8_t programHalfword(uint16_t hwrd,ptr addr);
uint8_t programPage(uint8_t pagenr,uint16_t* data,uint16_t cnt);

#endif /* FLASH_H_ */
