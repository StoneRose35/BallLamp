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
#include "hardware/regs/pwm.h"

// --------------------
// - pin declarations
// --------------------
#define CS_SDCARD 5
#define CS_DISPLAY 12
#define MISO 20
#define MOSI 19
#define SCK 18
#define DISPLAY_RESET 24
#define DISPLAY_CD 25
#define DISPLAY_BACKLIGHT 8 

// -----------------------
// - SCK Divider values
// -----------------------
#define SCK_SDCARD_INIT 199 // SPI clock divider for SD-Card initialization
#define SCK_SDCARD_MEDIUM 5 // resulting in 10 MHz clock rate
#define SCK_DISPLAY_SLOW 5 // resulting in 10 MHz spi clock for display

// ------------------------
// - register definitions
// ------------------------
#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))
#define SSPCR0 ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPCR0_OFFSET))
#define SSPCR1 ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPCR1_OFFSET))
#define SSPCPSR ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPCPSR_OFFSET))
#define SSPDR ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPDR_OFFSET))
#define SSPSR ((volatile uint32_t*)(SPI0_BASE +  SPI_SSPSR_OFFSET))
#define MISO_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*MISO))
#define MOSI_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*MOSI))
#define SCK_PIN_CNTR  ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SCK))
#define MISO_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*MISO))
#define CS_SDCARD_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*CS_SDCARD))
#define CS_DISPLAY_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*CS_DISPLAY))

#define DISPLAY_RESET_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*DISPLAY_RESET))
#define DISPLAY_CD_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*DISPLAY_CD))

#define PWM_CH0_CSR ((volatile uint32_t*)(PWM_BASE + PWM_CH0_CSR_OFFSET + 10*DISPLAY_BACKLIGHT))
#define PWM_CH0_DIV ((volatile uint32_t*)(PWM_BASE + PWM_CH0_DIV_OFFSET + 10*DISPLAY_BACKLIGHT))
#define PWM_CH0_CC ((volatile uint16_t*)(PWM_BASE + PWM_CH0_CC_OFFSET + 10*DISPLAY_BACKLIGHT + (DISPLAY_BACKLIGHT & 1)*2))
#define PWM_CH0_TOP ((volatile uint32_t*)(PWM_BASE + PWM_CH0_TOP_OFFSET + 10*DISPLAY_BACKLIGHT))
#define DISPLAY_BACKLIGHT_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*DISPLAY_BACKLIGHT))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))

#endif

void initSpi();
void sendDummyBytes(uint16_t cnt,uint8_t targetbyte);
uint8_t sendCommand(uint8_t* cmd,uint8_t* resp,uint16_t len);
uint8_t initSdCard();
uint8_t readSector(uint8_t* sect, uint32_t address);

void csDisableDisplay();
void csEnableDisplay();
void csDisableSDCard();
void csEnableSDCard();

void initDisplay();
uint8_t blankScreen();
void setBacklight(uint8_t);

#define SD_CARD_VERSION_2 1
#define SD_CARD_VERSION_1 5

#define R1_IDLE 0
#define R1_ILLEGAL_COMMAND 2
#define R1_CRC_ERROR 3

#define ERROR_TIMEOUT 1
#define ERROR_CARD_UNRESPONSIVE 2
#define ERROR_V2_CMD8_RESPONSE 3
#define ERROR_ILLEGAL_COMMAND 4
#define ERROR_READ_FAILURE 5

#define ST7735_MADCTL_MY 7
#define ST7735_MADCTL_MX 6
#define ST7735_MADCTL_MV 5
#define ST7735_MADCTL_ML 4
#define ST7735_MADCTL_RGB 3
#define ST7735_MADCTL_MH 2
#endif