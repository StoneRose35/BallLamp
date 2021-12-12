/*
 * led_timer.c
 *
 *  Created on: Jul 27, 2021
 *      Author: philipp
 */
#ifdef RP2040_FEATHER

#include "neopixelDriver.h"
#include "system.h"

#include "gen/pioprogram.h"
#include "hardware/regs/pio.h"
#include "hardware/pio.h"
#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/dma.h"
#include "hardware/regs/m0plus.h"


volatile uint8_t sendState=SEND_STATE_INITIAL;

// framerate timer irq function
// should update sendState every frame to notify the mainloop that a new  color info should be sent
// and should also update sendState once clocking in the colors is done a.k.a. when decompressArray can be called earliest
void isr_pio0_irq0_irq7()
{
	if((*PIO_IRQ & (1 << 0)) == (1 << 0)) // got irq 0, a frame has passed
	{
		if (sendState != SEND_STATE_DATA_READY)
		{
			sendState = SEND_STATE_BUFFER_UNDERRUN;
		}
		else
		{
			*PIO_IRQ |= (1 << 0);
			sendState = SEND_STATE_RTS;
		}
	}
}



// function to be called to switch off clocking out colors 
// this happens when the dma has transferred the necessary color information
// for all lamps 
void isr_dma_irq0_irq11()
{

	if ((*DMA_INTS0 & (1<<0))==(1 << 0)) // if from channel 0
	{
		// clear interrupt
		*DMA_INTS0 |= (1<<0);

		// disable dma
		*DMA_CH0_CTRL_TRIG &= ~(1 << 0);

		sendState = SEND_STATE_SENT;
	}
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
	/**
	 * @brief DMA Setup
	 * 
	 */
	// read from the raw data array in memory
	*DMA_CH0_READ_ADDR = (uint32_t)rawdata_ptr;
	//place data into the TX fifo of PIO0's SM0
	*DMA_CH0_WRITE_ADDR = PIO0_BASE+PIO_TXF0_OFFSET;
	//increase read address at each transfer, select DREQ0 (DREQ_PIO0_TX0) as data sed request
	*DMA_CH0_CTRL_TRIG |= (1 << DMA_CH0_CTRL_TRIG_INCR_READ_LSB) | (0 << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB);
	//define the number of data sent
	*DMA_CH0_TRANS_COUNT = 3*N_LAMPS;
	//generate interrupt upon completion of the data transfer which happens on channel 0
	*DMA_INTE0 |= (1 << 0);

	/* 
	 * PIO Setup
	*/
	uint8_t instr_mem_cnt = 0;
	uint8_t instr_offset = 0;

	// enable the PIO0 block
    *RESETS &= ~(1 << RESETS_RESET_PIO0_LSB);
	while ((*RESETS_DONE & (1 << RESETS_RESET_PIO0_LSB)) == 0);

	// switch the neopixel pin to be controlled by the pio0
	*NEOPIXEL_PIN_CNTR =  6; 

    // enable side-set, set wrap top and wrap bottom
	*PIO_SM0_EXECCTRL |= (1 << PIO_SM0_EXECCTRL_SIDE_EN_LSB) 
	| ( ws2812_wrap_target + instr_mem_cnt << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB)
	| ( ws2812_wrap + instr_mem_cnt << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);

	//  do pull after 8 bits of have been shifted out, enable autopull
	*PIO_SM0_SHIFTCTRL |= (8 << PIO_SM1_SHIFTCTRL_PULL_THRESH_LSB) |(1 << PIO_SM1_SHIFTCTRL_AUTOPULL_LSB);

	// fill in instructions
	for(uint8_t c=0;c < ws2812_program.length;c++){
		*(PIO_INSTR_MEM + instr_mem_cnt++) = *(ws2812_program.instructions + c);
	}

    // set one sideset pin, base to the neopixel output pin
	*PIO_SM0_PINCTRL |= (1 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB) 
	| (NEOPIXEL_PIN << PIO_SM1_PINCTRL_SIDESET_BASE_LSB);

	// set counter, based on 128Mhz/(800kHz*10) -> 16
	*PIO_SM0_CLKDIV = 16 << PIO_SM0_CLKDIV_INT_LSB;

	// start PIO 0, state machine 0
	*PIO_CTRL |= (1 << PIO_CTRL_SM_ENABLE_LSB+0);

	/*
	 * configure state machine 1 which serves as a frame timer
	*/
	// disable side-set, set wrap top and wrap bottom
	*PIO_SM1_EXECCTRL |= (0 << PIO_SM0_EXECCTRL_SIDE_EN_LSB) 
	| ( frametimer_wrap_target + instr_mem_cnt << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB)
	| ( frametimer_wrap + instr_mem_cnt << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);

	//  do pull after 32 bits of have been shifted out, disable autopull
	*PIO_SM0_SHIFTCTRL |= (32 << PIO_SM1_SHIFTCTRL_PULL_THRESH_LSB) |(0 << PIO_SM1_SHIFTCTRL_AUTOPULL_LSB);

	// fill in instructions
	for(uint8_t c=0;c < ws2812_program.length;c++){
		*(PIO_INSTR_MEM + instr_mem_cnt++) = *(ws2812_program.instructions + c);
	}

	// write the appropriate wait value to the transmit fifo
	*PIO_SM1_TXF = 4400000;

	//enable interrupt from pio0 sm1
	*PIO_INTE |= (1 << PIO_IRQ0_INTS_SM1_LSB);

	// start PIO 0, state machine 1
	*PIO_CTRL |= (1 << PIO_CTRL_SM_ENABLE_LSB+1);

	// enable interrupts 7 and 11
	*NVIC_ISER = (1 << 7) | (1 << 11);
}

/* non-blocking function which initiates a data transfer to the neopixel array
 * be aware that rawdata should not be modified as long as the data transfer is ongoing
 * */
void sendToLed()
{
	// reset address
	*DMA_CH0_READ_ADDR = (uint32_t)rawdata_ptr;

	// enable dma
	*DMA_CH0_CTRL_TRIG |= (1 << 0);
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
