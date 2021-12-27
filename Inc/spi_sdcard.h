#ifndef _SPI_SDCARD_H
#define _SPI_SDCARD_H

#include <stdint.h>

#ifdef STM32
#endif

#ifdef RP2040_FEATHER
#include "hardware/regs/addressmap.h"
#include "hardware/regs/spi.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/sio.h"

#define CS 21
#define MISO 20
#define MOSI 19
#define SCK 18


#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))
#define SSPCR0 ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPCR0_OFFSET))
#define SSPCR1 ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPCR1_OFFSET))
#define SSPCPSR ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPCPSR_OFFSET))
#define SSPDR ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPDR_OFFSET))
#define SSPSR ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPSR_OFFSET))
#define MISO_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*MISO))
#define MOSI_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*MOSI))
#define SCK_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SCK))
#define CS_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SCK))
#define MOSI_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*MOSI))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))

#endif

void initSpi();
void sendDummyBytes(uint16_t cnt);
void sendCommand(uint8_t* cmd,uint8_t* resp,uint16_t len);

uint8_t initSdCard();

#define SD_CARD_VERSION_2 1
#define SD_CARD_VERSION_1 5

#define R1_IDLE 0
#define R1_ILLEGAL_COMMAND 2
#define R1_CRC_ERROR 3
#endif