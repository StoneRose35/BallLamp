#ifndef _HEATER_H_
#define _HEATER_H_

#define HEATER  6
#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))

#define PWM_CH1_CSR ((volatile uint32_t*)(PWM_BASE + PWM_CH0_CSR_OFFSET + 10*HEATER))
#define PWM_CH1_DIV ((volatile uint32_t*)(PWM_BASE + PWM_CH0_DIV_OFFSET + 10*HEATER))
#define PWM_CH1_CC ((volatile uint16_t*)(PWM_BASE + PWM_CH0_CC_OFFSET + 10*HEATER + (HEATER & 1)*2))
#define PWM_CH1_TOP ((volatile uint32_t*)(PWM_BASE + PWM_CH0_TOP_OFFSET + 10*HEATER))
#define HEATER_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*HEATER))


void setHeater(uint16_t val);
void initHeater();

#endif