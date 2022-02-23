
#include "hardware/regs/addressmap.h"
#include "hardware/regs/pio.h"
#include "hardware/regs/io_bank0.h"
#include "gen/pio0_pio.h"
#include "i2s.h"
#include "dma.h"

//__attribute__((aligned (AUDIO_BUFFER_SIZE*2*2)))
static uint16_t i2sDoubleBuffer[AUDIO_BUFFER_SIZE*2*2];
static volatile  uint32_t dbfrPtr; 
volatile uint32_t audioState;

void initI2S()
{
    uint16_t instr_mem_cnt = 0;
	uint16_t first_instr_pos;

	audioState = 0;
 
    first_instr_pos = instr_mem_cnt;
    // disable optional side-set, set wrap top and wrap bottom
	*PIO1_SM0_EXECCTRL = (0 << PIO_SM0_EXECCTRL_SIDE_EN_LSB) 
	| ( (i2s_write_wrap_target + first_instr_pos) << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB)
	| ( (i2s_write_wrap + first_instr_pos) << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);

	//  pull after 16 bits have been read, disable autopull, join fifo
	*PIO1_SM0_SHIFTCTRL = (16 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) 
                           |(0 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) 
                           | (1 << PIO_SM0_SHIFTCTRL_FJOIN_TX_LSB) 
						   | (0 << PIO_SM0_SHIFTCTRL_OUT_SHIFTDIR_LSB);

	// fill in instructions
	// offset the jump instruction by position of the first command since the jump addresses
	// are relative to the program
	for(uint8_t c=0;c < i2s_write_program.length;c++){
		*(PIO1_INSTR_MEM + instr_mem_cnt++) = (*(i2s_write_program.instructions + c) & 0xe000)==0 ?
		 *(i2s_write_program.instructions + c) + first_instr_pos : *(i2s_write_program.instructions + c);
	}


	// define DS18B20 as a set pin and as the lowest input pin
	// define one set pin
	*PIO1_SM0_PINCTRL = 
	(2 << PIO_SM0_PINCTRL_SIDESET_COUNT_LSB)
    | (I2S_DATA_PIN << PIO_SM2_PINCTRL_OUT_BASE_LSB)
	| (I2S_BCK_PIN << PIO_SM2_PINCTRL_SIDESET_BASE_LSB)
    ;

	// switch control of i2s pins to pio
	*I2S_BCK_PIN_CNTR = 6;
    *I2S_DATA_PIN_CNTR = 6;
    *I2S_WS_PIN_CNTR = 6;

	// set clock divider : 120Mhz/78 (.125)
	*PIO1_SM0_CLKDIV = (I2S_CLKDIV_INT << PIO_SM0_CLKDIV_INT_LSB);// | (I2S_CLKDIV_FRAC << PIO_SM0_CLKDIV_FRAC_LSB);

	// jump to first instruction
	*PIO1_SM0_INSTR = first_instr_pos;

    // set pin directions to output
    *PIO1_SM0_INSTR = 0xE09F;

	// initialize DMA
	*DMA_CH2_WRITE_ADDR = (uint32_t)PIO1_SM0_TXF;
	dbfrPtr = 0;
	*DMA_CH2_READ_ADDR = dbfrPtr + (uint32_t)i2sDoubleBuffer;
	*DMA_CH2_TRANS_COUNT = AUDIO_BUFFER_SIZE;
	*DMA_CH2_CTRL_TRIG = (8 << DMA_CH2_CTRL_TRIG_TREQ_SEL_LSB) 
						| (1 << DMA_CH2_CTRL_TRIG_INCR_READ_LSB) 
						| (2 << DMA_CH2_CTRL_TRIG_DATA_SIZE_LSB) // always read left and right at once
						| (0 << DMA_CH2_CTRL_TRIG_EN_LSB);

    // start PIO 1, state machine 0
	*PIO1_CTRL |= (1 << (PIO_CTRL_SM_ENABLE_LSB+0));
}

void toggleAudioBuffer()
{
	dbfrPtr += AUDIO_BUFFER_SIZE*2;
	dbfrPtr &= (AUDIO_BUFFER_SIZE*2-1);
	*DMA_CH2_READ_ADDR = dbfrPtr + (uint32_t)i2sDoubleBuffer;
	*DMA_CH2_TRANS_COUNT = AUDIO_BUFFER_SIZE; //elif rock spielen 
}

void enableAudioEngine()
{
	*DMA_CH2_CTRL_TRIG |= (1 << DMA_CH2_CTRL_TRIG_EN_LSB);
	audioState = (1 << AUDIO_STATE_ON);
}

void disableAudioEngine()
{
	*DMA_CH2_CTRL_TRIG &= ~(1 << DMA_CH2_CTRL_TRIG_EN_LSB);
	audioState = 0;
}

int16_t* getEditableBuffer()
{
	int16_t * otherBuffer;
	otherBuffer = (int16_t*)(((dbfrPtr + AUDIO_BUFFER_SIZE*2) & (AUDIO_BUFFER_SIZE*2-1)) + (uint32_t)i2sDoubleBuffer);
	return otherBuffer;
}