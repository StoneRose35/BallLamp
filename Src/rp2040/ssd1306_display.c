/**
 * @file ssd1306_display.c
 * @author philipp fuerholz
 * @brief driver for a 120*64 oled display driven by a ssd1306 interface using 4-pin spi
 * @version 0.1
 * @date 2022-03-10
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "ssd1306_display.h"
#include "systick.h"
#include "fonts/oled_font_5x7.h"

void initSsd1306Display()
{
    // get spi out of reset
    *RESETS |= (1 << RESETS_RESET_SPI0_LSB); 
	*RESETS &= ~(1 << RESETS_RESET_SPI0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_SPI0_LSB)) == 0);

    // get pads out of reset
    *RESETS |= (1 << RESETS_RESET_PADS_BANK0_LSB); 
	*RESETS &= ~(1 << RESETS_RESET_PADS_BANK0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_PADS_BANK0_LSB)) == 0);

    // wire up the spi
    *MOSI_PIN_CNTR = 1;
    *SCK_PIN_CNTR = 1;
    *CS_DISPLAY_PIN_CNTR = 1;

    *GPIO_OE |= (1 << DISPLAY_RESET);
    *GPIO_OE |= (1 << DISPLAY_CD); 
    *DISPLAY_CD_PIN_CNTR = 5;
    *DISPLAY_RESET_PIN_CNTR = 5;

    // configure control register 0: 8-bit data, 10 MHz Clock
    *SSPCR0 = (0x7 << SPI_SSPCR0_DSS_LSB) | (SCK_DISPLAY_SLOW << SPI_SSPCR0_SCR_LSB);
    // configure clock divider
    *SSPCPSR = 2;
    // configure control register 1: enable by setting synchronous operation
    *SSPCR1 = 1 << SPI_SSPCR1_SSE_LSB;



    // reset high
    *(GPIO_OUT + 1) = (1 << DISPLAY_RESET);
    waitSysticks(1);
    // reset low
    *(GPIO_OUT + 2) = (1 << DISPLAY_RESET);
    waitSysticks(1);
    // reset high
    *(GPIO_OUT + 1) = (1 << DISPLAY_RESET);
    waitSysticks(1);

    // send display on command
    *(GPIO_OUT + 2) = (1 << DISPLAY_CD);
    *SSPDR = 0xAF;
    while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) ); 
    waitSysticks(11);
}

/**
 * @brief Set the Cursor object
 * 
 * @param row goes from 0 to 7, defines the page to write
 * @param col columns, goes from 0 to 127
 */
void setCursor(uint8_t row, uint8_t col)
{
    *(GPIO_OUT + 2) = (1 << DISPLAY_CD);
    // set column, high nibble
    *SSPDR = 0x10 & (col >> 4);
    while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) ); 
    // set column, low nibble
    *SSPDR = 0x0 & (col & 0x0F);
    while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) );

    // set row / page
    *SSPDR = 0xB0 & row;
     while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) );

}

void ssd1306WriteChar(uint8_t row,uint8_t col,char chr)
{
    uint8_t fontIdx;
    setCursor(row,col*6);
    fontIdx = (uint8_t)chr - ' ';

    *(GPIO_OUT + 1) = (1 << DISPLAY_CD); // switch to data
    for (uint8_t c=0;c<5;c++)
    {
        *SSPDR = oled_font_5x7[fontIdx][c];
        while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) );
    }
    *SSPDR = 0x0;
    while ((*SSPSR & (1 << SPI_SSPSR_BSY_LSB))==(1 << SPI_SSPSR_BSY_LSB) );

}

void ssd1306WriteText(char * str,uint16_t posH,uint16_t posV)
{
    uint8_t cnt = 0;
    uint16_t hCurrent=posH;
    while(*(str+cnt) != 0)
    {
        ssd1306WriteChar(*(str+cnt),posV,hCurrent);

        hCurrent += 1;

        cnt++;
    }
}