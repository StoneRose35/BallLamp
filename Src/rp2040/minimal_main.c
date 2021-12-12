

#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/sio.h"
#include <stdlib.h>

#define GPIO13_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO13_CTRL_OFFSET))
#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define GPIO_OE ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_OFFSET))
#define GPIO_OUT ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OUT_OFFSET))

#define LED_PIN (13)

typedef unsigned int uint32_t; 

volatile int testarray[256];
volatile unsigned int answer_to_all_questions_of_the_universe = 42;
void blinkSetup();
void _sr35_delay(uint32_t val);
void _boot_stage_2();
void * dynamicArray;

int notmain()
{
	char a=0;

	blinkSetup();

	dynamicArray=malloc(24*sizeof(int));

	while(1)
	{

		*GPIO_OUT |= (1 << LED_PIN);
		_sr35_delay(6500000UL);
		*GPIO_OUT &= ~(1 << LED_PIN);
		_sr35_delay(6500000UL);
		a++;		
	}	
}

void blinkSetup()
{
    //switch on GPIO
    *RESETS &= ~(1 << 5);
	while ((*RESETS_DONE & (1 << 5)) == 0)
	{

	}

	*GPIO_OE &= ~(1 << LED_PIN);
	*GPIO_OUT &= ~(1 << LED_PIN);

    *GPIO13_CNTR =  5; // function 5 (SIO)

	*GPIO_OE |= (1 << LED_PIN);

}

