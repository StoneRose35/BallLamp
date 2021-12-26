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
typedef uint32_t* ptr;

#ifdef HARDWARE
inline ptr getFilesystemStart()
{
	extern uint8_t __filesystem_start;
	return (ptr)&__filesystem_start;
}

#else
	uint8_t fakeflash[8192];
	inline ptr getFilesystemStart()
	{
		return (ptr)fakeflash;
	}
#endif


void unlockFlash();
uint8_t erasePage(uint16_t pagenr);
uint8_t programHalfword(uint16_t hwrd,ptr addr);
uint8_t programPage(uint16_t pagenr,uint16_t* data,uint16_t cnt);

#endif /* FLASH_H_ */
