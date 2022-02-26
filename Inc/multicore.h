#ifndef _MULTICORE_H_
#define _MULTICORE_H_
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/m0plus.h"
#include "hardware/regs/sio.h"



#define SIO_FIFO_ST ((volatile uint32_t*)(SIO_BASE + SIO_FIFO_ST_OFFSET))
#define SIO_FIFO_RD ((volatile uint32_t*)(SIO_BASE + SIO_FIFO_RD_OFFSET))
#define SIO_FIFO_WR ((volatile uint32_t*)(SIO_BASE + SIO_FIFO_WR_OFFSET))

#define M0PLUS_VTOR ((volatile uint32_t*)(PPB_BASE + M0PLUS_VTOR_OFFSET))
#define NVIC_ISER ((volatile uint32_t*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET))
#define NVIC_ICER ((volatile uint32_t*)(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET))

void startCore1(void(*secondMain)());

void multicore_fifo_drain();

void __sev();

#endif