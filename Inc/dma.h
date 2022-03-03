#ifndef _DMA_H_
#define _DMA_H_
#include <stdint.h>

void initDMA();

#ifdef RP2040_FEATHER

#include "hardware/regs/addressmap.h"
#include "hardware/regs/dma.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/m0plus.h"

#define DMA_CH0_WRITE_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH0_WRITE_ADDR_OFFSET))
#define DMA_CH0_READ_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH0_READ_ADDR_OFFSET))
#define DMA_CH0_CTRL_TRIG ((volatile uint32_t*)(DMA_BASE+DMA_CH0_CTRL_TRIG_OFFSET))
#define DMA_CH0_TRANS_COUNT ((volatile uint32_t*)(DMA_BASE+DMA_CH0_TRANS_COUNT_OFFSET))

#define DMA_CH1_WRITE_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH1_WRITE_ADDR_OFFSET))
#define DMA_CH1_READ_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH1_READ_ADDR_OFFSET))
#define DMA_CH1_CTRL_TRIG ((volatile uint32_t*)(DMA_BASE+DMA_CH1_CTRL_TRIG_OFFSET))
#define DMA_CH1_TRANS_COUNT ((volatile uint32_t*)(DMA_BASE+DMA_CH1_TRANS_COUNT_OFFSET))

#define DMA_CH2_WRITE_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH2_WRITE_ADDR_OFFSET))
#define DMA_CH2_READ_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH2_READ_ADDR_OFFSET))
#define DMA_CH2_CTRL_TRIG ((volatile uint32_t*)(DMA_BASE+DMA_CH2_CTRL_TRIG_OFFSET))
#define DMA_CH2_TRANS_COUNT ((volatile uint32_t*)(DMA_BASE+DMA_CH2_TRANS_COUNT_OFFSET))
#define DMA_CH2_TRANS_COUNT_TRIG ((volatile uint32_t*)(DMA_BASE+DMA_CH2_TRANS_COUNT_OFFSET+0x14))

#define DMA_CH3_WRITE_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH3_WRITE_ADDR_OFFSET))
#define DMA_CH3_READ_ADDR ((volatile uint32_t*)(DMA_BASE+DMA_CH3_READ_ADDR_OFFSET))
#define DMA_CH3_CTRL_TRIG ((volatile uint32_t*)(DMA_BASE+DMA_CH3_CTRL_TRIG_OFFSET))
#define DMA_CH3_TRANS_COUNT ((volatile uint32_t*)(DMA_BASE+DMA_CH3_TRANS_COUNT_OFFSET))
#define DMA_CH3_TRANS_COUNT_TRIG ((volatile uint32_t*)(DMA_BASE+DMA_CH3_TRANS_COUNT_OFFSET+0x14))

#define DMA_INTE0 ((volatile uint32_t*)(DMA_BASE+DMA_INTE0_OFFSET))
#define DMA_INTS0 ((volatile uint32_t*)(DMA_BASE+DMA_INTS0_OFFSET))

#define RESETS ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_OFFSET))
#define RESETS_DONE ((volatile uint32_t*)(RESETS_BASE + RESETS_RESET_DONE_OFFSET))

#define NVIC_ISER ((volatile uint32_t*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET))
#endif
#endif

