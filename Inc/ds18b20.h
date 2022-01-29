#ifndef _DS18B20_H_
#define _DS18B20_H_
#include <stdint.h>

#define DS18B20_PIN 2

#define DS18B20_INSTR_MEM_OFFSET 10
#define DS18B20_CLKDIV 480 // 4 us clockdiv
#define DS18B20_CLKDIV_READ 120 // 1 us clock
#define DS18B20_RESET_CLKDIV 7200

#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define PIO_INSTR_MEM ((volatile uint32_t*)(PIO0_BASE+PIO_INSTR_MEM0_OFFSET))
#define PIO_CTRL ((volatile uint32_t*)(PIO0_BASE+PIO_CTRL_OFFSET))
#define PIO_IRQ ((volatile uint32_t*)(PIO0_BASE + PIO_IRQ_OFFSET))
#define PIO_FDEBUG ((volatile uint32_t*)(PIO0_BASE + PIO_FDEBUG_OFFSET))
#define PIO_FSTAT ((volatile uint32_t*)(PIO0_BASE + PIO_FSTAT_OFFSET))

#define PIO_SM2_EXECCTRL ((volatile uint32_t*)(PIO0_BASE+PIO_SM2_EXECCTRL_OFFSET))
#define PIO_SM2_SHIFTCTRL ((volatile uint32_t*)(PIO0_BASE+PIO_SM2_SHIFTCTRL_OFFSET))
#define PIO_SM2_PINCTRL ((volatile uint32_t*)(PIO0_BASE+PIO_SM2_PINCTRL_OFFSET))
#define PIO_SM2_CLKDIV ((volatile uint32_t*)(PIO0_BASE+PIO_SM2_CLKDIV_OFFSET))
#define PIO_SM2_TXF ((volatile uint32_t*)(PIO0_BASE+PIO_TXF2_OFFSET))
#define PIO_SM2_INSTR  ((volatile uint32_t*)(PIO0_BASE+PIO_SM2_INSTR_OFFSET))
#define PIO_SM2_RXF ((volatile uint32_t*)(PIO0_BASE+PIO_RXF2_OFFSET))

#define DS18B20_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*DS18B20_PIN))
#define DS18B20_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*DS18B20_PIN))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))

uint8_t initDs18b20();
uint8_t writeDs18b20(uint8_t cmd);
uint8_t readDs18b20();
uint8_t resetDs18b20();

uint16_t initTempConversion();
uint8_t readTemp(int16_t*);
uint8_t getTempReadState();
 #endif