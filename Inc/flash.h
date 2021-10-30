/*
 * flash.h
 *
 *  Created on: 30.10.2021
 *      Author: philipp
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "system.h"

void unlockFlash();
uint8_t erasePage(uint8_t pagenr);
uint8_t programHalfword(uint16_t hwrd,uint32_t addr);
uint16_t saveInFlash(uint16_t * data,uint32_t size,uint32_t offset);

#endif /* FLASH_H_ */
