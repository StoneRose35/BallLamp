
#include "dma.h"
#include "neopixelDriver.h"

extern volatile uint8_t sendState;
extern volatile uint32_t task;

void initDMA()
{
	// enable the dma block
	*RESETS |= (1 << RESETS_RESET_DMA_LSB);
    *RESETS &= ~(1 << RESETS_RESET_DMA_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_DMA_LSB)) == 0);

	// enable the dma interrupt by default
	*NVIC_ISER = (1 << 11);
}


// function to be called to switch off clocking out colors 
// this happens when the dma has transferred the necessary color information
// for all lamps 
void isr_dma_irq0_irq11()
{
	if ((*DMA_INTS0 & (1<<0))==(1 << 0)) // if from channel 0: neopixel  frame timer
	{
		// clear interrupt
		*DMA_INTS0 |= (1<<0);

		// disable dma channel 0
		*DMA_CH0_CTRL_TRIG &= ~(1 << 0);

		sendState = SEND_STATE_SENT;
	}
	else if ((*DMA_INTS0 & (1<<1))==(1 << 1)) // from channel 1: usb uart transmission done
	{
		*DMA_INTS0 |= (1<<1);
		task |= (1 << TASK_USB_CONSOLE_TX);
	}
	return;
}