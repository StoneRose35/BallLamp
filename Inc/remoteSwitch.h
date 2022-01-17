#ifndef _REMOTE_SWITCH_H_
#define _REMOTE_SWITCH_H_
#include "stdint.h"

#define REMOTESWITCH_PIN 14

#define PIO_INSTR_MEM ((volatile uint32_t*)(PIO0_BASE+PIO_INSTR_MEM0_OFFSET))
#define PIO_CTRL ((volatile uint32_t*)(PIO0_BASE+PIO_CTRL_OFFSET))
#define PIO_IRQ ((volatile uint32_t*)(PIO0_BASE + PIO_IRQ_OFFSET))
#define PIO_FDEBUG ((volatile uint32_t*)(PIO0_BASE + PIO_FDEBUG_OFFSET))
#define PIO_FSTAT ((volatile uint32_t*)(PIO0_BASE + PIO_FSTAT_OFFSET))

#define PIO_SM3_EXECCTRL ((volatile uint32_t*)(PIO0_BASE+PIO_SM3_EXECCTRL_OFFSET))
#define PIO_SM3_SHIFTCTRL ((volatile uint32_t*)(PIO0_BASE+PIO_SM3_SHIFTCTRL_OFFSET))
#define PIO_SM3_PINCTRL ((volatile uint32_t*)(PIO0_BASE+PIO_SM3_PINCTRL_OFFSET))
#define PIO_SM3_CLKDIV ((volatile uint32_t*)(PIO0_BASE+PIO_SM3_CLKDIV_OFFSET))
#define PIO_SM3_TXF ((volatile uint32_t*)(PIO0_BASE+PIO_TXF3_OFFSET))
#define PIO_SM3_INSTR  ((volatile uint32_t*)(PIO0_BASE+PIO_SM3_INSTR_OFFSET))
#define PIO_SM3_RXF ((volatile uint32_t*)(PIO0_BASE+PIO_RXF3_OFFSET))

#define REMOTESWITCH_PIN_CNTR ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET + 8*REMOTESWITCH_PIN))
#define REMOTESWITCH_PAD_CNTR ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET + 4*REMOTESWITCH_PIN))

#define REMOTESWITCH_CLKDIV 26640
#define REMOTESWITCH_INSTR_MEM_OFFSET 20

void sendRemoteSwitchCommand(uint32_t cmd);
void initRemoteSwitch();
void remoteSwitchOn();
void remoteSwitchOff();

#endif