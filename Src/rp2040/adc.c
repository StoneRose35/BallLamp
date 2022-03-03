#include "adc.h"
#include "dma.h"
#include "system.h"
static uint16_t audioInDoubleBuffer[AUDIO_INPUT_BUFFER_SIZE*2];
static volatile  uint32_t dbfrPtr; 
volatile uint32_t audioInputState;

/**
 * @brief initializes the adc, the usb pll must be on before the initialization can take place
 * 
 */
void initAdc()
{
    // enable the adc clock
    *CLK_ADC_CTRL |= (1 << CLOCKS_CLK_ADC_CTRL_ENABLE_LSB);

    // de-reset adc block
    *RESETS |= (1 << RESETS_RESET_ADC_LSB);
    *RESETS &= ~(1 << RESETS_RESET_ADC_LSB);
    while ((*RESETS_DONE & (1 << RESETS_RESET_ADC_LSB)) == 0);

    // enable adc
    *ADC_CS |= (1 << ADC_CS_EN_LSB);
}

/**
 * @brief reads the given adc channel (0 to 3)
 * 
 * @param channelnr the selected channel, 0 is GPIO26 up to 3 /GPIO29
 * @return uint16_t the 12-bit raw adc result
 */
uint16_t readChannel(uint8_t channelnr)
{   
    uint32_t adcChannelPadSettings;
    uint16_t res;
    adcChannelPadSettings = *(PADS_ADC0 + channelnr);
 // set IE high and OD low for the pad register of the channel
    *(PADS_ADC0 + channelnr) |= (1 << PADS_BANK0_GPIO26_IE_LSB);
    *(PADS_ADC0 + channelnr) &= ~(1 << PADS_BANK0_GPIO26_OD_LSB);

    while((*ADC_CS & (1 << ADC_CS_READY_LSB))==0);
    *ADC_CS |= (channelnr << ADC_CS_AINSEL_LSB); // choose channel

    // start the conversion
    *ADC_CS |= (1 << ADC_CS_START_ONCE_LSB);

    // wait until ready
    while((*ADC_CS & (1 << ADC_CS_READY_LSB))==0);

    // read the result
    res= (uint16_t)(*ADC_RESULT & 0xFFF);

    // reset the state of the pad
    *(PADS_ADC0 + channelnr) = adcChannelPadSettings;
    return res;
}

void initDoubleBufferedReading(uint8_t channelnr)
{
    // setup pads
    *(PADS_ADC0 + channelnr) |= (1 << PADS_BANK0_GPIO26_IE_LSB);
    *(PADS_ADC0 + channelnr) &= ~(1 << PADS_BANK0_GPIO26_OD_LSB);

    // set samping rate
    *ADC_DIV=((F_ADC_USB/AUDIO_SAMPLING_RATE) - 1) << 8; 

    // enable fifo and dreq and set thresh to 1
    *ADC_FCS = (1 << ADC_FCS_EN_LSB) | (1 << ADC_FCS_DREQ_EN_LSB) | (1 << ADC_FCS_THRESH_LSB); 

    // enable irq for channel 3
    *DMA_INTE0 |= (1 << 3);

    // initialize DMA
	*DMA_CH3_READ_ADDR = (uint32_t)ADC_FIFO;
	dbfrPtr = 0;
	*DMA_CH3_WRITE_ADDR = dbfrPtr + (uint32_t)audioInDoubleBuffer;
	*DMA_CH3_TRANS_COUNT = AUDIO_INPUT_BUFFER_SIZE;
	*DMA_CH3_CTRL_TRIG = (36 << DMA_CH3_CTRL_TRIG_TREQ_SEL_LSB) 
						| (1 << DMA_CH3_CTRL_TRIG_INCR_WRITE_LSB) 
						| (1 << DMA_CH3_CTRL_TRIG_DATA_SIZE_LSB) // adc is single channel 16 bit
						| (0 << DMA_CH3_CTRL_TRIG_EN_LSB);

}

void enableAudioInput()
{
    *DMA_CH3_TRANS_COUNT = AUDIO_INPUT_BUFFER_SIZE;
    dbfrPtr = 0;
	*DMA_CH3_WRITE_ADDR = dbfrPtr + (uint32_t)audioInDoubleBuffer;
    *ADC_CS |= (1 << ADC_CS_START_MANY_LSB); 
    *DMA_CH3_CTRL_TRIG |= (1 << DMA_CH3_CTRL_TRIG_EN_LSB);
}

void toggleAudioInputBuffer()
{
    dbfrPtr += AUDIO_INPUT_BUFFER_SIZE*2;
	dbfrPtr &= (AUDIO_INPUT_BUFFER_SIZE*2*2-1);
	*DMA_CH3_WRITE_ADDR = dbfrPtr + (uint32_t)audioInDoubleBuffer;
    *DMA_CH3_TRANS_COUNT_TRIG = AUDIO_INPUT_BUFFER_SIZE; // write to alias 1 to trigger dma on writing transmission count
}

uint16_t* getReadableAudioBuffer()
{
    uint16_t * otherBuffer;
	otherBuffer = (uint16_t*)(((dbfrPtr + AUDIO_INPUT_BUFFER_SIZE*2) & (AUDIO_INPUT_BUFFER_SIZE*2*2-1)) + (uint32_t)audioInDoubleBuffer);
	return otherBuffer;
}
