#ifndef _I2S_H_
#define _I2S_H_
#include <stdint.h>

#define I2S_INPUT

#define I2S_WS_PIN 12
#define I2S_BCK_PIN 11
#define I2S_DATA_PIN 10
#define I2S_DATA_IN_PIN 9
#define I2S_MCLK_PIN 8
#define I2S_DEBUG_PIN 6

// 120MHz / 78.125 = 48kHz*2*16
#define I2S_CLKDIV_INT 36
#define I2S_CLKDIV_FRAC 16

#define I2S_CLKDIV_DBL_INT 19
#define I2S_CLKDIV_DBL_FRAC 136

#define I2S_CLKDIV_MCLK_INT 4
#define I2S_CLKDIV_MCLK_FRAC 226

#define AUDIO_BUFFER_SIZE 16 // in stereo 16bit samples, thus size of one audio buffer in bytes is this value*4

#define AUDIO_STATE_BUFFER_UNDERRUN 1
#define AUDIO_STATE_ON 0

#define PIO1_CTRL ((volatile uint32_t*)(PIO1_BASE+PIO_CTRL_OFFSET))
#define PIO1_INSTR_MEM ((volatile uint32_t*)(PIO1_BASE+PIO_INSTR_MEM0_OFFSET))
#define PIO1_INTE ((volatile uint32_t*)(PIO1_BASE + PIO_IRQ0_INTE_OFFSET))
#define PIO1_INTF ((volatile uint32_t*)(PIO1_BASE + PIO_IRQ0_INTF_OFFSET))
#define PIO1_IRQ ((volatile uint32_t*)(PIO1_BASE + PIO_IRQ_OFFSET))
#define PIO1_IRQ_FORCE ((volatile uint32_t*)(PIO1_BASE + PIO_IRQ_FORCE_OFFSET))
#define PIO1_FSTAT ((volatile uint32_t*)(PIO1_BASE + PIO_FSTAT_OFFSET))

#define PIO1_INSTR_MEM ((volatile uint32_t*)(PIO1_BASE+PIO_INSTR_MEM0_OFFSET))
#define PIO1_CTRL ((volatile uint32_t*)(PIO1_BASE+PIO_CTRL_OFFSET))
#define PIO1_IRQ ((volatile uint32_t*)(PIO1_BASE + PIO_IRQ_OFFSET))
#define PIO1_FDEBUG ((volatile uint32_t*)(PIO1_BASE + PIO_FDEBUG_OFFSET))
#define PIO1_FSTAT ((volatile uint32_t*)(PIO1_BASE + PIO_FSTAT_OFFSET))

#define PIO1_SM0_EXECCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM0_EXECCTRL_OFFSET))
#define PIO1_SM0_SHIFTCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM0_SHIFTCTRL_OFFSET))
#define PIO1_SM0_PINCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM0_PINCTRL_OFFSET))
#define PIO1_SM0_CLKDIV ((volatile uint32_t*)(PIO1_BASE+PIO_SM0_CLKDIV_OFFSET))
#define PIO1_SM0_TXF ((volatile uint32_t*)(PIO1_BASE+PIO_TXF0_OFFSET))
#define PIO1_SM0_INSTR  ((volatile uint32_t*)(PIO1_BASE+PIO_SM0_INSTR_OFFSET))
#define PIO1_SM0_RXF ((volatile uint32_t*)(PIO1_BASE+PIO_RXF0_OFFSET))

#define PIO1_SM1_EXECCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM1_EXECCTRL_OFFSET))
#define PIO1_SM1_SHIFTCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM1_SHIFTCTRL_OFFSET))
#define PIO1_SM1_PINCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM1_PINCTRL_OFFSET))
#define PIO1_SM1_CLKDIV ((volatile uint32_t*)(PIO1_BASE+PIO_SM1_CLKDIV_OFFSET))
#define PIO1_SM1_TXF ((volatile uint32_t*)(PIO1_BASE+PIO_TXF1_OFFSET))
#define PIO1_SM1_INSTR  ((volatile uint32_t*)(PIO1_BASE+PIO_SM1_INSTR_OFFSET))
#define PIO1_SM1_RXF ((volatile uint32_t*)(PIO1_BASE+PIO_RXF1_OFFSET))

#define PIO1_SM2_EXECCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM2_EXECCTRL_OFFSET))
#define PIO1_SM2_SHIFTCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM2_SHIFTCTRL_OFFSET))
#define PIO1_SM2_PINCTRL ((volatile uint32_t*)(PIO1_BASE+PIO_SM2_PINCTRL_OFFSET))
#define PIO1_SM2_CLKDIV ((volatile uint32_t*)(PIO1_BASE+PIO_SM2_CLKDIV_OFFSET))
#define PIO1_SM2_TXF ((volatile uint32_t*)(PIO1_BASE+PIO_TXF2_OFFSET))
#define PIO1_SM2_INSTR  ((volatile uint32_t*)(PIO1_BASE+PIO_SM2_INSTR_OFFSET))
#define PIO1_SM2_RXF ((volatile uint32_t*)(PIO1_BASE+PIO_RXF2_OFFSET))

#define I2S_WS_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2S_WS_PIN))
#define I2S_WS_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*I2S_WS_PIN))
#define I2S_BCK_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2S_BCK_PIN))
#define I2S_BCK_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*I2S_BCK_PIN))
#define I2S_DATA_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2S_DATA_PIN))
#define I2S_DATA_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*I2S_DATA_PIN))
#define I2S_DATA_IN_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2S_DATA_IN_PIN))
#define I2S_DATA_IN_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*I2S_DATA_IN_PIN))
#define I2S_MCLK_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2S_MCLK_PIN))
#define I2S_MCLK_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*I2S_MCLK_PIN))
#define I2S_DEBUG_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*I2S_DEBUG_PIN))
#define I2S_DEBUG_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*I2S_DEBUG_PIN))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))

void initI2S();
void enableAudioEngine();
void disableAudioEngine();
void toggleAudioBuffer();
int16_t* getEditableAudioBuffer();
#ifdef I2S_INPUT
int16_t* getInputAudioBuffer();
#endif
#endif