/*
 * types.h
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */

#include "systemChoice.h"

#ifndef TYPES_H_
#define TYPES_H_


#ifdef HARDWARE
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef short int16_t;
typedef volatile uint32_t reg;
typedef uint32_t ptr;
#else
#include <stdint.h>
typedef unsigned long ptr;
#endif


#endif /* TYPES_H_ */
