/*
 * core.c
 *
 *  Created on: Oct 26, 2021
 *      Author: philipp
 */


#include "core.h"
#include "types.h"

void enableFpu()
{
	*CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
}
