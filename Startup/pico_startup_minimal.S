
#define PPB_BASE _u(0xe0000000)
#define M0PLUS_VTOR_OFFSET _u(0x0000ed08)

.syntax unified
.cpu cortex-m0plus
.thumb

.section .vectors, "ax"
.align 2

.global __vectors //, __VECTOR_TABLE
//__VECTOR_TABLE:
__vectors:
.word __StackTop
.word _reset_handler
.word isr_nmi
.word isr_hardfault
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_svcall
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_pendsv
.word isr_systick
.word isr_timer_irq0
.word isr_timer_irq1
.word isr_timer_irq2
.word isr_timer_irq3
.word isr_pwm_wrap_irq4
.word isr_usbctrl_irq5
.word isr_xip_irq6
.word isr_pio0_irq0_irq7
.word isr_pio0_irq1_irq8
.word isr_pio1_irq0_irq9
.word isr_pio1_irq1_irq10
.word isr_dma_irq0_irq11
.word isr_dma_irq1_irq12
.word isr_io_irq_bank0_irq13
.word isr_io_irq_qspi_irq14
.word isr_sio_irq_proc0_irq15
.word isr_sio_irq_proc1_irq16
.word isr_clocks_irq17
.word isr_spi0_irq18
.word isr_spi1_irq19
.word isr_uart0_irq20
.word isr_uart1_irq21
.word isr_adc_fifo_irq22
.word isr_i2c0_irq23
.word isr_i2c1_irq24
.word isr_rtc_irq25
.word isr_irq26
.word isr_irq27
.word isr_irq28
.word isr_irq29
.word isr_irq30
.word isr_irq31

/* standard isr declarations as weak function */
.macro std_isr name
.weak \name
.type \name,%function
\name:
.endm

/* breakpoint isr, they are called upon a fault */
.macro bkpt_isr name
.weak \name
.type \name,%function
\name:
    bkpt #0
.endm

bkpt_isr isr_nmi
bkpt_isr isr_hardfault
bkpt_isr isr_invalid
bkpt_isr isr_svcall
bkpt_isr isr_pendsv
bkpt_isr isr_systick

std_isr isr_timer_irq0
std_isr isr_timer_irq1
std_isr isr_timer_irq2
std_isr isr_timer_irq3
std_isr isr_pwm_wrap_irq4
std_isr isr_usbctrl_irq5
std_isr isr_xip_irq6
std_isr isr_pio0_irq0_irq7
std_isr isr_pio0_irq1_irq8
std_isr isr_pio1_irq0_irq9
std_isr isr_pio1_irq1_irq10
std_isr isr_dma_irq0_irq11
std_isr isr_dma_irq1_irq12
std_isr isr_io_irq_bank0_irq13
std_isr isr_io_irq_qspi_irq14
std_isr isr_sio_irq_proc0_irq15
std_isr isr_sio_irq_proc1_irq16
std_isr isr_clocks_irq17
std_isr isr_spi0_irq18
std_isr isr_spi1_irq19
std_isr isr_uart0_irq20
std_isr isr_uart1_irq21
std_isr isr_adc_fifo_irq22
std_isr isr_i2c0_irq23
std_isr isr_i2c1_irq24
std_isr isr_rtc_irq25
std_isr isr_irq26
std_isr isr_irq27
std_isr isr_irq28
std_isr isr_irq29
std_isr isr_irq30
std_isr isr_irq31


.section .reset, "ax"

.type _entry_point,%function
.thumb_func
.global _entry_point
_entry_point:
    movs r0, #0
    // set the interrupt vector table to address 0 (the very beginning)
    ldr r1, =0xe000ed08 //=(PPB_BASE + M0PLUS_VTOR_OFFSET)
    str r0, [r1]
    
    // load the stack top ( __StackTop( from address 0 (in r0) and the reset handler (_reset_handler) address into r2
    ldmia r0!, {r1, r2}
    msr msp, r1 // set the stack pointer to stack top
    bx r2 // jump/branch to _reset_handler


.type _reset_handler,%function
.thumb_func
_reset_handler:
    ldr r1, =main
    blx r1

