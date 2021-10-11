/*
 * intFunctions.c
 *
 *  Created on: 11.10.2021
 *      Author: philipp
 */


#include "intFunctions.h"
#include "system.h"

int compareUint8(const void* a,const void* b)
{
	return (int)(*(uint8_t*)a - *(uint8_t*)b);
}
