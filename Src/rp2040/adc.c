#include "adc.h"

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