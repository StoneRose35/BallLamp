#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/io_bank0.h"
#include "debugLed.h"

void initDebugLed()
{
	*GPIO_OE &= ~(1 << LED_PIN);
	*GPIO_OUT &= ~(1 << LED_PIN);

    *DEBUG_LED_PIN_CNTR =  5; // function 5 (SIO)
	*GPIO_OE |= (1 << LED_PIN);
}

void DebugLedOn()
{
    *(GPIO_OUT+1) = (1 << LED_PIN);
}
void DebugLedOff()
{
    *(GPIO_OUT+2) = (1 << LED_PIN);
}

void DebugLedToggle()
{
    *(GPIO_OUT+3) = (1 << LED_PIN);
}