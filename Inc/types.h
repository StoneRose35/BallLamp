/*
 * types.h
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */

#ifndef TYPES_H_
#define TYPES_H_

#ifdef STM32
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef short int16_t;
typedef volatile uint32_t reg;
#endif

#endif /* TYPES_H_ */
