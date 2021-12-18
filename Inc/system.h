/**
 * system.h
 * contains microprocessor-independent information on the lamp system
 * such as the number of lamp attached and the intended framerate
 * while the system clock frequency can be entirely defined within neopixelDriver.h
 * the framerate settings should be entirely deriveable from FRAMERATE
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "types.h"

#define N_LAMPS (20)
#define FRAMERATE (30) // frames or lamp images per second

// settings regarding the specific neopixels used, adjust depending on the actual neopixel chip
#define WS2818_FREQ (800000) // 1/(0.4es + 0.85us)
#define WS2818_SHORT (2500000) // 1/0.4us
#define WS2818_LONG  (1176470)

#ifdef STM32
#define F_BUS (64000000)
#define F_APB1 F_BUS/2
#define WS2818_CNT F_BUS/WS2818_FREQ
#define WS2818_SHORT_CNT F_BUS/WS2818_SHORT
#define WS2818_LONG_CNT F_BUS/WS2818_LONG
#define REMAINING_WAITTIME F_BUS/FRAMERATE-F_BUS/WS2818_FREQ*24*N_LAMPS

#define I2C_ADDRESS (15)
#define FLASH_PAGE_SIZE 2048
#define FLASH_PAGE_SIZE_BIT 11
#endif

#ifdef RP2040_FEATHER
#define FLASH_PAGE_SIZE 4096
#define FLASH_PAGE_SIZE_BIT 12
#endif



#define PHASE_INC (12)



#define TASK_USB_CONSOLE 0
#define TASK_BT_CONSOLE 1

#define CONTEXT_USB 0
#define CONTEXT_BT 1

#define FLASH_HEADER_SIZE 32

/**
 * @brief a simple rgb structure, is not aligned to be streamed directly to the neopixels
 * 
 */
typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB;

typedef union
{
	RGB rgb;
	uint32_t stream;
} RGBStream;

#endif /* SYSTEM_H_ */
