/*
 * led_timer.c
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */
#ifdef RP2040_FEATHER

#include "neopixelDriver.h"
#include "hardware/regs/pio.h"
#include "hardware/pio.h"
#include "hardware/regs/addressmap.h"
#include "gen/pioprogram.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/sio.h"
#include "system.h"

#define NEOPIXEL_PIN 27

#define PIO_CTRL ((volatile uint32_t*)PIO0_BASE+PIO_CTRL_OFFSET)
#define PIO_SM0_EXECCTRL ((volatile uint32_t*)PIO0_BASE+PIO_SM0_EXECCTRL_OFFSET)
#define PIO_SM0_SHIFTCTRL ((volatile uint32_t*)PIO0_BASE+PIO_SM0_SHIFTCTRL_OFFSET)
#define PIO_SM1_EXECCTRL ((volatile uint32_t*)PIO0_BASE+PIO_SM1_EXECCTRL_OFFSET)
#define PIO_SM1_SHIFTCTRL ((volatile uint32_t*)PIO0_BASE+PIO_SM1_SHIFTCTRL_OFFSET)
#define PIO_SM0_PINCTRL ((volatile uint32_t*)PIO0_BASE+PIO_SM0_PINCTRL_OFFSET)
#define PIO_SM0_CLKDIV ((volatile uint32_t*)PIO0_BASE+PIO_SM0_CLKDIV_OFFSET)
#define PIO_INSTR_MEM ((volatile uint16_t*)PIO0_BASE+PIO_INSTR_MEM0_OFFSET)

#define NEOPIXEL_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*NEOPIXEL_PIN))
#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))





volatile uint8_t sendState=SEND_STATE_INITIAL;

// framerate timer irq function
// should update sendState every frame to notify the mainloop that a new  color info should be sent
// and should also update sendState once clocking in the colors is done a.k.a. when decompressArray can be called earliest
void TIM2_IRQHandler()
{

}

// function to be called to switch off clocking out colors 
void DMA1_CH2_IRQHandler()
{

	return;
}

// converts the structured color data for all lamp (frame) into a streamable array handleable by the driver
// should modify the array pointed to by rawdata_ptr
void decompressRgbArray(RGBStream * frame,uint8_t length)
{
	uint8_t cnt = 0;
	uint8_t cbfr;
	uint16_t rdata_cnt = 0;
	*(rawdata_ptr + rdata_cnt) = 0;
	rdata_cnt=1;
	for(cnt=0;cnt<length;cnt++)
	{
		// refer to specific hardware implementation to check which color comes first
		cbfr = (*(frame+cnt)).rgb.g;
		*(rawdata_ptr + rdata_cnt++) = (*(frame+cnt)).rgb.g;
		cbfr = (*(frame+cnt)).rgb.r;
		*(rawdata_ptr + rdata_cnt++) = (*(frame+cnt)).rgb.r;
		cbfr = (*(frame+cnt)).rgb.b;
		*(rawdata_ptr + rdata_cnt++) = (*(frame+cnt)).rgb.r;

		//*(rawdata_ptr + rdata_cnt++) = 0; // add a zero to allow word-wise data shifting
	}
	sendState = SEND_STATE_DATA_READY;
}

/*
 * Initiates the framerate timer, rawdata transfer 
   mechanism (usually dma) and the hardware driver necessary for operating the neopixel array
 * should be called once after startup
 */
void initTimer()
{
	/* 
	 * PIO Setup
	*/

	// enable the PIO0 block
    *RESETS &= ~(1 << RESETS_RESET_PIO0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_PIO0_LSB)) == 0);

	// switch the neopixel pin to be controlled by the pio0
	*NEOPIXEL_PIN_CNTR =  6; 

    // enable side-set, set wrap top and wrap bottom
	*PIO_SM0_EXECCTRL |= (1 << PIO_SM0_EXECCTRL_SIDE_EN_LSB) 
	| ( ws2812_wrap_target << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB)
	| ( ws2812_wrap << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);

	//  do pull after 8 bits of have been shifted out, enable autopull
	*PIO_SM0_SHIFTCTRL |= (8 << PIO_SM1_SHIFTCTRL_PULL_THRESH_LSB) |(1 << PIO_SM1_SHIFTCTRL_AUTOPULL_LSB);

	// fill in instructions
	for(uint8_t c=0;c < ws2812_program.length;c++){
		*(PIO_INSTR_MEM + c) = *(ws2812_program.instructions + c);
	}

    // set one sideset pin, base to the neopixel output pin
	*PIO_SM0_PINCTRL |= (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) 
	| (NEOPIXEL_PIN << PIO_SM1_PINCTRL_SIDESET_BASE_LSB);

	// start PIO 0, state machine 0
	*PIO_CTRL |= (1 << PIO_CTRL_SM_ENABLE_LSB);

	/*
	 * configure state machine 1 which serves as a frame timer
	*/
	// enable side-set, set wrap top and wrap bottom
	*PIO_SM1_EXECCTRL |= (0 << PIO_SM0_EXECCTRL_SIDE_EN_LSB) 
	| ( frame_timer_wrap_target << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB)
	| ( frame_timer_wrap << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);


}

/* non-blocking function which initiates a data transfer to the neopixel array
 * be aware that rawdata should not be modified as long as the data transfer is ongoing
 * */
void sendToLed()
{

}

uint8_t getSendState()
{
	return sendState;
}

void setSendState(uint8_t s)
{
	sendState = s;
}


#endif
