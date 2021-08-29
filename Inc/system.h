/*
 * system.h
 *
 *  Created on: 28.07.2021
 *      Author: philipp
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#define F_BUS (64000000)
#define WS2818_FREQ (800000) // 1/(0.4es + 0.85us)
#define WS2818_SHORT (2500000) // 1/0.4us
#define WS2818_LONG  (1176470)

#define WS2818_CNT F_BUS/WS2818_FREQ
#define WS2818_SHORT_CNT F_BUS/WS2818_SHORT
#define WS2818_LONG_CNT F_BUS/WS2818_LONG

#define N_LAMPS (1)

#define PHASE_INC (12)

#define I2C_ADDRESS (15)

typedef struct
{
	uint8_t g;
	uint8_t r;
	uint8_t b;
	uint8_t res;
} RGB;

typedef union
{
	RGB rgb;
	uint32_t stream;
} RGBStream;

#endif /* SYSTEM_H_ */
