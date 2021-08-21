/*
 * led_timer.h
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */
#include "types.h"
#include "system.h"

#ifndef LED_TIMER_H_
#define LED_TIMER_H_


typedef struct {
	volatile uint32_t CR1;
	volatile uint32_t CR2;
	volatile uint32_t SMCR;
	volatile uint32_t DIER;
	volatile uint32_t SR;
	volatile uint32_t EGR;
	volatile uint32_t CCMR1;
	volatile uint32_t CCMR2;
	volatile uint32_t CCER;
	volatile uint32_t CNT;
	volatile uint32_t PSC;
	volatile uint32_t ARR;
	volatile uint32_t reserved;
	volatile uint32_t CCR1;
	volatile uint32_t CCR2;
	volatile uint32_t CCR3;
	volatile uint32_t CCR4;
	volatile uint32_t reserved1;
	volatile uint32_t DCR;
	volatile uint32_t DMAR;
} TIM23TypeDef;

typedef struct {
	volatile uint32_t CCR;
	volatile uint32_t CNDTR;
	volatile uint32_t CPAR;
	volatile uint32_t CMAR;
	volatile uint32_t reserved;
} DMAChannelTypeDef;

typedef struct {
	volatile uint32_t ISR;
	volatile uint32_t IFCR;
	DMAChannelTypeDef CHANNEL[7];
} DMATypeDef;

typedef struct {
	volatile uint32_t MODER;
	volatile uint32_t OTYPER;
	volatile uint32_t OSPEEDR;
	volatile uint32_t PUPDR;
	volatile uint32_t IDR;
	volatile uint32_t ODR;
	volatile uint32_t BSRR;
	volatile uint32_t LCKR;
	volatile uint32_t AFRL;
	volatile uint32_t AFRH;
	volatile uint32_t BRR;

} GPIOTypeDef;

// GPIO bits
#define INPUT (0)
#define OUTPUT (1)
#define AF (2)
#define ANALOG (3)


#define CEN (0)
#define OC1M (4)
#define OC1PE (3)
#define OC1FE (2)
#define TIM2EN (0)
#define PINA0POS (0)
#define UIE (0)
#define CC1DE (9)
#define UDE (8)

// DMA CCR bits
#define MEM2MEM (14)
#define PL (12)
#define MSIZE (10)
#define PSIZE (8)
#define MINC (7)
#define PINC (6)
#define CIRC (5)
#define DIR (4)
#define TEIE (3)
#define HTIE (2)
#define TCIE (1)
#define EN (0)

#define RCC_APB1ENR ((volatile uint32_t*)0x4002101CUL)
#define RCC_AHBENR ((volatile uint32_t*)0x40021014UL) //0x14
//#define GPIOA_AFRL ((volatile uint32_t*)0x48000020UL)
#define GPIOA ((GPIOTypeDef*)0x48000000UL)
#define GPIOB ((GPIOTypeDef*)0x48000400UL)
#define TIM2 ((TIM23TypeDef*)0x40000000UL)
#define DMA ((DMATypeDef*)0x40020000UL)
#define NVIC_ISER0 ((volatile uint32_t*)0xE000E100UL)

void initTimer();
void decompressRgbArray(RGBStream * frame,uint8_t length);
void sendToLed();

extern RGBStream * frame;
extern uint32_t clr_cnt;
extern uint32_t bit_cnt;
extern uint32_t * rawdata_ptr;

#endif /* LED_TIMER_H_ */
