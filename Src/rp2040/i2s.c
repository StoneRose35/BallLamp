
#include "hardware/regs/addressmap.h"
#include "hardware/regs/pio.h"
#include "hardware/regs/io_bank0.h"
#include "gen/pio0_pio.h"
#include "i2s.h"


void initI2S()
{
    uint16_t instr_mem_cnt = 0;
	uint16_t first_instr_pos;

 
    first_instr_pos = instr_mem_cnt;
    // disable optional side-set, set wrap top and wrap bottom
	*PIO1_SM0_EXECCTRL = (0 << PIO_SM0_EXECCTRL_SIDE_EN_LSB) 
	| ( (i2s_write_wrap_target + first_instr_pos) << PIO_SM0_EXECCTRL_WRAP_BOTTOM_LSB)
	| ( (i2s_write_wrap + first_instr_pos) << PIO_SM0_EXECCTRL_WRAP_TOP_LSB);

	//  pull after 16 bits have been read, disable autopull, join fifo
	*PIO1_SM0_SHIFTCTRL |= (16 << PIO_SM0_SHIFTCTRL_PULL_THRESH_LSB) 
                           |(0 << PIO_SM0_SHIFTCTRL_AUTOPULL_LSB) 
                           | (1 << PIO_SM0_SHIFTCTRL_FJOIN_TX_LSB);

	// fill in instructions
	// offset the jump instruction by position of the first command since the jump addresses
	// are relative to the program
	for(uint8_t c=0;c < ds18b20read_program.length;c++){
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

    // start PIO 1, state machine 0
	*PIO1_CTRL |= (1 << (PIO_CTRL_SM_ENABLE_LSB+0));
}