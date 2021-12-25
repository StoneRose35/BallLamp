/**
 * @file system.h
 * @author Philipp Fuerholz (fuerholz@gmx.ch)
 * @brief contains microprocessor-independent information on the lamp system
 * such as the number of lamp attached and the intended framerate
 * while the system clock frequency can be entirely defined within neopixelDriver.h
 * the framerate settings should be entirely deriveable from FRAMERATE
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "types.h"
#include "systemChoice.h"

#define N_LAMPS (20) //!< the number of neopixel the system contains 

#define FRAMERATE (30) //!< frames or lamp images per second

// settings regarding the specific neopixels used, adjust depending on the actual neopixel chip
#define WS2818_FREQ (800000) //!< frequency of the neopixel interface, is 400000  (400 kHz)for older ones and 800000 (800 kHz)for new ones
#define WS2818_SHORT (2500000) //!< 1/pulse length of the short neopixel pulse
#define WS2818_LONG  (1176470) //!< 1/pulse length of the long neopixel pulse

#ifdef HARDWARE
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
#define FLASH_PAGE_SIZE 4096 //!< size in bytes of a flash page, a flash page is the smallest eraseable flash unit
#define FLASH_PAGE_SIZE_BIT 12 //!< bit position of the flash page size, i.e. 2^FLASH_PAGE_SIZE_BIT = FLASH_PAGE_SIZE
#endif

#else
#define FLASH_PAGE_SIZE 16
#define FLASH_PAGE_SIZE_BIT 4
#endif



#define PHASE_INC (12) //!<phase increment for the hue shift function in main.c



#define TASK_USB_CONSOLE 0 //!< bit position of the task bit array, set: the task for handling the usb console should be called
#define TASK_BT_CONSOLE 1 //!< bit position of the task bit array, set: the task for handling the bluetooth console should be called

#define CONTEXT_USB 0 //!<bit position: if set output of prints is routed to the USB console
#define CONTEXT_BT 1 //!<bit position: if set output of prints is routed to the bluetooth console

#define FLASH_HEADER_SIZE 32 //!< size in bytes of a non-overwriteable area at the beginning of the Flash section

/**
 * @brief a simple rgb structure, is not aligned to be streamed directly to the neopixels
 *
 */
typedef struct
{
	uint8_t r; //!< Red
	uint8_t g; //!< Green
	uint8_t b; //!< Blue
} RGB;

/**
 * @brief overlay of a potentially streamable color information with the color structure, currently unused
 * 
 */
typedef union
{
	RGB rgb; //!< the color struct
	uint32_t stream; //!< the streamable content of the color struct
} RGBStream;

#endif /* SYSTEM_H_ */
