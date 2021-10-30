/*
 * system.h
 *
 *  Created on: 28.07.2021
 *      Author: philipp
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#define N_LAMPS (20)
#define FRAMERATE (30) // frames or lamp images

#define F_BUS (64000000)
#define F_APB1 F_BUS/2
#define WS2818_FREQ (800000) // 1/(0.4es + 0.85us)
#define WS2818_SHORT (2500000) // 1/0.4us
#define WS2818_LONG  (1176470)

#define WS2818_CNT F_BUS/WS2818_FREQ
#define WS2818_SHORT_CNT F_BUS/WS2818_SHORT
#define WS2818_LONG_CNT F_BUS/WS2818_LONG
#define REMAINING_WAITTIME F_BUS/FRAMERATE-F_BUS/WS2818_FREQ*24*N_LAMPS



#define PHASE_INC (12)

#define I2C_ADDRESS (15)

#define TASK_USB_CONSOLE 0
#define TASK_BT_CONSOLE 1

#define CONTEXT_USB 0
#define CONTEXT_BT 1

#ifndef STM32
#include <stdint.h>
typedef unsigned long ptr;
#else
#include "types.h"
typedef unsigned int ptr;
#endif

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t res;
} RGB;

typedef union
{
	RGB rgb;
	uint32_t stream;
} RGBStream;

#endif /* SYSTEM_H_ */
