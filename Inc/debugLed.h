#ifndef _DEBUG_LED_H_
#define _DEBUG_LED_H_
#include <stdint.h>

#define LED_PIN (25) // 13 on rp2040 feather, 25 on rpi pico boards

#define DEBUG_LED_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*LED_PIN))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))


void initDebugLed();

void DebugLedOn();
void DebugLedOff();
void DebugLedToggle();
#endif