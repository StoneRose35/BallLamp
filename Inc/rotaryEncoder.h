#ifndef _ROTARY_ENCODER_H_
#define _ROTARY_ENCODER_H_
#include <stdint.h>
#ifdef RP2040_FEATHER

#define ENCODER_1 7
#define ENCODER_2 10
#define SWITCH 22
#define BACKLIGHT 8

#define ROTARY_ENCODER_DEBOUNCE 1

#define ENCODER_1_PIN_CNTR  ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*ENCODER_1))
#define ENCODER_1_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*ENCODER_1))
#define ENCODER_1_INTE ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_PROC0_INTE0_OFFSET + (((4*ENCODER_1) & 0xFFE0) >> 3)))
#define ENCODER_1_INTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_INTR0_OFFSET + (((4*ENCODER_1) & 0xFFE0) >> 3)))
#define ENCODER_1_EDGE_LOW (((4*ENCODER_1) & 0x1F)+2)
#define ENCODER_1_EDGE_HIGH (((4*ENCODER_1) & 0x1F)+3)

#define ENCODER_2_PIN_CNTR  ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*ENCODER_2))
#define ENCODER_2_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*ENCODER_2))

#define ENCODER_2_IN ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_IN_OFFSET))

#define SWITCH_PIN_CNTR  ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SWITCH))
#define SWITCH_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*SWITCH))
#define SWITCH_INTE ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_PROC0_INTE0_OFFSET + (((4*SWITCH) & 0xFFE0) >> 3)))
#define SWITCH_INTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_INTR0_OFFSET + (((4*SWITCH) & 0xFFE0) >> 3)))
#define SWITCH_EDGE_LOW (((4*SWITCH) & 0x1F)+2)
#define SWITCH_EDGE_HIGH (((4*SWITCH) & 0x1F)+3)

#define NVIC_ISER ((volatile uint32_t*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET))

#endif

#ifdef STM32
#endif

void initRotaryEncoder();

uint32_t getEncoderValue();

uint8_t getSwitchValue();

#endif

