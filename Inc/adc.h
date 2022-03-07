#ifndef _ADC_H_
#define _ADC_H_
#include <stdint.h>

#include "hardware/regs/addressmap.h"
#include "hardware/regs/adc.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/clocks.h"
#include "hardware/regs/pads_bank0.h"

#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define CLK_ADC_CTRL ((volatile uint32_t*)(CLOCKS_BASE + CLOCKS_CLK_ADC_CTRL_OFFSET))
#define CLK_ADC_DIV ((volatile uint32_t*)(CLOCKS_BASE + CLOCKS_CLK_ADC_DIV_OFFSET))

#define ADC_CS ((volatile uint32_t*)(ADC_BASE + ADC_CS_OFFSET))
#define ADC_DIV ((volatile uint32_t*)(ADC_BASE + ADC_DIV_OFFSET))
#define ADC_RESULT ((volatile uint32_t*)(ADC_BASE + ADC_RESULT_OFFSET))
#define ADC_FCS ((volatile uint32_t*)(ADC_BASE + ADC_FCS_OFFSET))
#define ADC_FIFO ((volatile uint32_t*)(ADC_BASE + ADC_FIFO_OFFSET))

#define PADS_ADC0 ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO26_OFFSET))


#define AUDIO_INPUT_BUFFER_SIZE 16
#define AUDIO_SAMPLING_RATE 48000

#define AUDIO_STATE_INPUT_ON 2
#define AUDIO_STATE_INPUT_BUFFER_OVERRUN 3

void initAdc();
uint16_t readChannel(uint8_t channelnr);
void initDoubleBufferedReading(uint8_t channelnr);

void startConversion();
void core1IrqSync();

void enableAudioInput(uint8_t freeRunning);
void toggleAudioInputBuffer();
uint16_t * getReadableAudioBuffer();

#endif