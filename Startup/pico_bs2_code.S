
#include "hardware/regs/addressmap.h"

.syntax unified
.cpu cortex-m0plus
.thumb

.section .boot_code2, "ax"
.align 2


// TODO: properly set up QSPI for fast flash access


// initialize the stack pointer
ldr r1, =__StackTop    
ldr r1,[r1]
msr msp, r1 

// jump off to second stage bootloader, which is at the 0x100-boundary from XIP (0x1000 0000)
ldr r1, =(XIP_BASE + 0x100) 
bx r1




