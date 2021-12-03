
#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/resets.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/ssi.h"

#define GPIO13_CNTR (IO_BANK0_BASE + IO_BANK0_GPIO13_CTRL_OFFSET)
#define RESETS (RESETS_BASE + RESETS_RESET_OFFSET)
#define RESETS_DONE (RESETS_BASE + RESETS_RESET_DONE_OFFSET)

#define GPIO_OE (SIO_BASE + SIO_GPIO_OE_OFFSET)
#define GPIO_OUT (SIO_BASE + SIO_GPIO_OUT_OFFSET)
#define GPIO_OUT_SET (SIO_BASE + SIO_GPIO_OUT_SET_OFFSET)

#define LED_PIN (13)

.syntax unified
.cpu cortex-m0plus
.thumb

.section .boot_code2, "ax"
.align 2


// TODO: properly set up QSPI for flash access



// try to switch on GPIO 13 directly from memory
    //switch on GPIO
/*
//*RESETS &= ~(1 << 5);
ldr r4, =RESETS
ldr r3, =0xFFFFFFFF - (1 << 5) // 0xFFFFDFFF
ldr r2, [r4]
ands r2, r3
str r1,[r4]


//while ((*RESETS_DONE & (1 << 5)) != 0)
r_done:
    ldr r4, =RESETS_DONE
    ldr r3,[r4]
    ldr r1,=(1 << 5)
    tst r3,r1
    bne r_done
 
//*GPIO_OE &= ~(1 << LED_PIN);
ldr r5,=GPIO_OE
ldr r3,[r5]
ldr r1,=(0xFFFFFFFF-(1 << LED_PIN))
ands r3,r1
str r3,[r5]

//*GPIO_OUT &= ~(1 << LED_PIN);
ldr r4,=GPIO_OUT
ldr r3,[r4]
ldr r1, =(0xFFFFFFFF-(1 << LED_PIN)) 
ands r3,r1
str r1,[r4]

//*GPIO13_CNTR |=  5; // function 5 (SIO)
ldr r4,=GPIO13_CNTR
ldr r3,[r4]
adds r3,#5
str r3,[r4]

//*GPIO_OE |= (1 << LED_PIN);
ldr r3,[r5]
ldr r4,=(1 << LED_PIN)
orrs r3,r4
str r3,[r5]

// switch on led
ldr r4,=GPIO_OUT_SET
ldr r3,=(1 << LED_PIN)
str r3,[r4]

*/
/* trying to initialize  the xip flash using default single channel spi mode*/



#define CMD_READ 0x03


// set xip registers
#define SSI_ENR (XIP_SSI_BASE + SSI_SSIENR_OFFSET)
#define SSI_BAUDR (XIP_SSI_BASE + SSI_BAUDR_OFFSET)
#define SSI_CNTR0 (XIP_SSI_BASE + SSI_CTRLR0_OFFSET)
#define SSI_CNTR1 (XIP_SSI_BASE + SSI_CTRLR1_OFFSET)
#define SSI_SPI_CNTR0 (XIP_SSI_BASE + SSI_SPI_CTRLR0_OFFSET)

#define SSI_TMOD (8)
#define SSI_DFS_32 (16) 
#define PICO_FLASH_SPI_CLKDIV 4
#define XIP_CMD 24
#define ADDR_L 2

// disable ssi
ldr r3, =SSI_ENR
movs r4, #0
str r4,[r3]

// set baud rate
ldr r3,=SSI_BAUDR
movs r4,PICO_FLASH_SPI_CLKDIV // set clock, division of 1 since the ROSC 
          // is supposed to run at ~6.5MHz and the GD25Q06CSIG
          // supports up to 120MHz
str r4,[r3]

// set SSI_CNTR0
ldr r3, =SSI_CNTR0
ldr r4, =((31 << SSI_DFS_32) | (3 << SSI_TMOD))
str r4,[r3]

// set SSI_SPI_CNTR0
ldr r3, =SSI_SPI_CNTR0
ldr r4, =((0x03 << XIP_CMD) | (6 << ADDR_L))

// dummy read
ldr r3, =SSI_CNTR1
movs r4, #0x0
str r4, [r3]

// enable ssi
ldr r3, =SSI_ENR
movs r4, #1
str r4,[r3]


// jump off to second stage bootloader, which is at the 0x100-boundary from XIP (0x1000 0000)
// initialize the stack pointer
ldr r1, =(XIP_BASE + 0x100)
ldr r2,[r1]
msr msp, r2

// branch to _start_c_runtime
ldr r1, =(XIP_BASE + 0x104) 
ldr r2,[r1]
bx r2




