/*
 * types.h
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */

#ifndef TYPES_H_
#define TYPES_H_

#ifdef STM32
#define ARM_PLATFORM
#endif
#ifdef RPI_PICO
#define ARM_PLATFORM
#endif

#ifdef ARM_PLATFORM
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
