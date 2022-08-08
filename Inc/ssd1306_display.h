#ifndef _SSD1306_DISPLAY_H_
#define _SSD1036_DISPLAY_H_
#include <stdint.h>

#include "hardware/regs/addressmap.h"
#include "hardware/regs/spi.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/pads_bank0.h"
#include "hardware/regs/sio.h"

void initSsd1306Display();


void ssd1306WriteChar(uint8_t row,uint8_t col,char chr);
void ssd1306WriteText(const char * str,uint8_t posH,uint8_t posV);
void ssd1306DisplayByteArray(uint8_t row,uint8_t col,uint8_t *arr,uint16_t arrayLength);
void ssd1306DisplayImage(uint8_t px,uint8_t py,uint8_t sx,uint8_t sy,uint8_t * img);
void ssd1306DisplayImageStandardAdressing(uint8_t px,uint8_t py,uint8_t sx,uint8_t sy,uint8_t * img);
void ssd1306ClearDisplay();

#define SSD1306_CS_DISPLAY 21
#define SSD1306_SCK 18
#define SSD1306_MOSI 19
#define SSD1306_DISPLAY_CD 14
#define SSD1306_DISPLAY_RESET 15

#define SSD1306_SCK_DISPLAY_SLOW 10 // resulting in 5 MHz spi clock for display

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

#define SSD1306_MOSI_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SSD1306_MOSI))
#define SSD1306_CS_DISPLAY_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SSD1306_CS_DISPLAY))
#define SSD1306_DISPLAY_CD_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SSD1306_DISPLAY_CD))
#define SSD1306_SCK_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SSD1306_SCK))
#define SSD1306_DISPLAY_RESET_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*SSD1306_DISPLAY_RESET))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))

#endif