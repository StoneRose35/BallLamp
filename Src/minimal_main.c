

#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/sio.h"

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
void delay(uint32_t val);

int main()
{
	blinkSetup();

	while(1)
	{

		char a=0;
		while (a!= 0xFF)
		{
			a = a+3;
			testarray[a]=a+2;
			if (a == 5 && answer_to_all_questions_of_the_universe < 50)
			{
				testarray[0] = testarray[64];
			}
		}

		*GPIO_OUT |= (1 << LED_PIN);
		delay(125000000);
		*GPIO_OUT &= (1 << LED_PIN);
		delay(125000000);
		
	}	
}

void blinkSetup()
{
    //switch on GPIO
    *RESETS &= ~(1 << 5);
	while ((*RESETS_DONE & (1 << 5)) != 0)
	{

	}

	*GPIO_OE &= ~(1 << LED_PIN);
	*GPIO_OUT &= ~(1 << LED_PIN);

    *GPIO13_CNTR |=  5; // function 5 (SIO)

	*GPIO_OE |= (1 << LED_PIN);

}

void delay(uint32_t val)
{
	while(val > 0)
	{
		val--;
	}
}

