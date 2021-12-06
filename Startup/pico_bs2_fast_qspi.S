
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sio.h"
#include "hardware/regs/ssi.h"

#define CMD_READ_QUAD 0xEB
#define CMD_WRITE_ENABLE 0x06

#define PICO_FLASH_SPI_CLKDIV 1

// register offsets
#define SSI_TMOD 8
#define SSI_DFS_32 16 
#define SSI_FRF 21
#define SSI_WAIT_CYCLES 11
#define ADDR_L 2
#define INSTR_L 8
#define TRANS_TYPE 0
#define XIP_CMD 24

#define SSI_ENR (XIP_SSI_BASE + SSI_SSIENR_OFFSET)
#define SSI_SR (XIP_SSI_BASE + SSI_SR_OFFSET)
#define SSI_BAUDR (XIP_SSI_BASE + SSI_BAUDR_OFFSET)
#define SSI_CNTR0 (XIP_SSI_BASE + SSI_CTRLR0_OFFSET)
#define SSI_CNTR1 (XIP_SSI_BASE + SSI_CTRLR1_OFFSET)
#define SSI_SPI_CNTR0 (XIP_SSI_BASE + SSI_SPI_CTRLR0_OFFSET)
#define SSI_DR0 (XIP_SSI_BASE + SSI_DR0_OFFSET)

.syntax unified
.cpu cortex-m0plus
.thumb

.section .ram_exec, "ax"
.align 2

.global _boot_stage_2
.type _boot_stage_2,%function
.thumb_func
_boot_stage_2:

// initialize the stack pointer
//ldr r1, =(XIP_BASE + 0x100)
//ldr r2,=__StackTop
//msr msp, r2

push {lr}


ldr r3, =XIP_SSI_BASE
// disable ssi
//ldr r3, =SSI_ENR
movs r4, #0
str r4,[r3, #SSI_SSIENR_OFFSET]

// set baud rate
movs r4,#2
str r4,[r3, #SSI_BAUDR_OFFSET]

/************** 
 * PART 1 
 **************/
// set 8 bit data frame, mode to Tx and Rx 
//ldr r3, =SSI_CNTR0

ldr r4, =(7 << SSI_DFS_32)
str r4,[r3, #SSI_CTRLR0_OFFSET]

// enable ssi
//ldr r3, =SSI_ENR
movs r4, #1
str r4,[r3, #SSI_SSIENR_OFFSET]

// TEST read status register 2
movs r4,#0x35
str r4, [r3, #SSI_DR0_OFFSET]
bl wait_for_transmit_done
ldr r4, [r3, #SSI_DR0_OFFSET]

// write bit 9 in the status register
//ldr r3, =SSI_DR0
movs r4, #CMD_WRITE_ENABLE
str r4,[r3, #SSI_DR0_OFFSET]

bl wait_for_transmit_done

// dummy read
//ldr r3,=SSI_DR0
ldr r4,[r3, #SSI_DR0_OFFSET]

// write status register 2
movs r4,#0x31
str r4,[r3, #SSI_DR0_OFFSET]
movs r4,#2 // write byte 9 (bit 1 of second byte)
str r4,[r3, #SSI_DR0_OFFSET]

bl wait_for_transmit_done

// empty read buffer
//ldr r3,=SSI_DR0
ldr r4,[r3, #SSI_DR0_OFFSET]
ldr r4,[r3, #SSI_DR0_OFFSET]

// read status register 1, wait until WIP (bit 0) is off
check_status:
movs r4,#0x05
str r4,[r3, #SSI_DR0_OFFSET]

bl wait_for_transmit_done

//ldr r3,=SSI_DR0
ldr r4,[r3, #SSI_DR0_OFFSET]
movs r2,#1
ands r2,r4
bne check_status

// configuration of the flash status register is done, now 
// configure the ssi for fast qspi mode

// disable ssi
//ldr r3, =SSI_ENR
movs r4, #0
str r4,[r3, #SSI_SSIENR_OFFSET]

// set SSI_CNTR0: 32 bit data frame size, eeprom mode, quad spi frame format
//ldr r3, =SSI_CNTR0
ldr r4, =((31 << SSI_DFS_32) | (3 << SSI_TMOD) | (2 << SSI_FRF))
str r4,[r3, #SSI_CTRLR0_OFFSET]

// read one data frame at the time
//ldr r3, =SSI_CNTR1
movs r4, #0x0
str r4, [r3,#SSI_CTRLR1_OFFSET]

/************** 
 * PART 2 
 **************/
// read a random first word to skip sending the read command subsequenty

// configure spi_cntr0: addr + mode 32 bits size, 2 wait cycles betwenn cmd/address/mode and 
// data, 8 bit instruction, command in 1bit/clock, address and data in 4bits/clock
//ldr r3, =SSI_SPI_CNTR0
ldr r4, =((8 << ADDR_L) | (4 << SSI_WAIT_CYCLES) | (2 << INSTR_L) | (1 << TRANS_TYPE))
ldr r2,=SSI_SPI_CNTR0
str r4,[r2]

// enable ssi
//ldr r3, =SSI_ENR
movs r4, #1
str r4,[r3, #SSI_SSIENR_OFFSET]


// read from address 0, set continuous read
//ldr r3,=SSI_DR0
ldr r4,=CMD_READ_QUAD
str r4,[r3, #SSI_DR0_OFFSET] // send command
movs r4,#0x20
str r4,[r3, #SSI_DR0_OFFSET] // send address + mode

bl wait_for_transmit_done

// TEST
ldr r4, [r3, #SSI_DR0_OFFSET] // check agaist the bootstage2.S, the first 4 bytes should be recognizeable

// disable ssi
//ldr r3, =SSI_ENR
movs r4, #0
str r4,[r3, #SSI_SSIENR_OFFSET]

/************** 
 * PART 3 
 **************/
 
// configure spi_cntr0: addr + mode 32 bits size, 2 wait cycles betwenn cmd/address/mode and 
// data, 8 bit instruction, command in 1bit/clock, address and data in 4bits/clock
//ldr r3, =SSI_SPI_CNTR0
ldr r4, =((32 << XIP_CMD) | (4 << SSI_WAIT_CYCLES) | (0 << INSTR_L) | (2 << TRANS_TYPE) | (8 << ADDR_L))
str r4,[r2]

// enable ssi
//ldr r3, =SSI_ENR
movs r4, #1
str r4,[r3, #SSI_SSIENR_OFFSET]


/*
// jump off to second stage bootloader, which is at the 0x100-boundary from XIP (0x1000 0000)
// initialize the stack pointer
ldr r1, =(XIP_BASE + 0x100)
ldr r2,[r1]
msr msp, r2

// branch to _start_c_runtime
ldr r1, =(XIP_BASE + 0x104) 
ldr r2,[r1]
bx r2
*/


// jump off to second stage bootloader or to the position in the link register
// when the link register pushed earlier is not 0
//, which is at the 0x100-boundary from XIP (0x1000 0000)
 
pop {r2}
//cmp r2,#0
//bne back_to_code
// branch to _start_c_runtime
ldr r1, =(XIP_BASE + 0x104) 
ldr r2,[r1]
back_to_code:
blx r2


.align 4
.type wait_for_transmit_done,%function
wait_for_transmit_done:
push {r2,r3,r4,lr}
ssi_wait:
ldr r3,=SSI_SR
ldr r4,[r3]
ldr r2,=(1 << 2)
ands r2,r4
beq ssi_wait
ldr r2,=(1 << 0)
ands r2,r4
bne ssi_wait
pop {r2,r3,r4,pc}
