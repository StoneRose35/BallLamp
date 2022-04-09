#ifndef _GPIO_H_
#define _GPIO_H_
#include <stdint.h>
#define PULL_STATE_NONE 0
#define PULL_STATE_UP 1
#define PULL_STATE_DOWN 2

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))
#define GPIO_IN ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_IN_OFFSET))

void setAsOuput(uint8_t pinNr);
void setAsInput(uint8_t pinNr, uint8_t PullState);

uint8_t readPin(uint8_t pinNr);
void setPin(uint8_t pinNr,uint8_t value);

#endif