

MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 8192k  /* 8MB on rp2040 feather */
    RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
}

_estack = ORIGIN(RAM) + LENGTH(RAM);
_heap_size_min = 0x1000; /* 4k of heap size minimum */
_stack_size_min = 0x400; /* 1k of protected minimum stack */


SECTIONS
{
    /* Second stage bootloader is prepended to the image. It must be 256 bytes big
       and checksummed. It is usually built by the boot_stage2 target
       in the Raspberry Pi Pico SDK
    */


    .boot2 : {
        __boot2_start__ = .;
        KEEP (*(.boot2))
        __boot2_end__ = .;
    } > FLASH
    
    /* used for pre-compiling the 256 bytes of the boot stage 2 code*/
    .boot2_code : {
    	*(.boot2_code*)
    } > FLASH

    ASSERT(__boot2_end__ - __boot2_start__ == 256,
        "ERROR: Pico second stage bootloader must be 256 bytes in size") 
        
    .startup2 : {
    	KEEP(*(.startup2*))
    } > FLASH
    
    .text : {
        KEEP (*(.vectors))
        KEEP (*(.reset))
        . = ALIGN(4);
    } > FLASH
    
    .rodata : {
        *(EXCLUDE_FILE(*libgcc.a: *libc.a:*lib_a-mem*.o *libm.a:) .rodata*)
        . = ALIGN(4);
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.flashdata*)))
        . = ALIGN(4);
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;
    
    
    . = ALIGN(4);
    
    __etext = .;
    
    .data : {
        __data_start__ = .;
        *(vtable)

        /* remaining .text and .rodata; i.e. stuff we exclude above because we want it in RAM */
        /* *(.text*) */
        /* . = ALIGN(4); */
        /* *(.rodata*) */
        /* . = ALIGN(4); */

        *(.data*)

        . = ALIGN(4);
        *(.after_data.*)
        . = ALIGN(4);
        /* preinit data */

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(SORT(.preinit_array.*)))
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        *(SORT(.fini_array.*))
        *(.fini_array)
        PROVIDE_HIDDEN (__fini_array_end = .);

        *(.jcr)
        . = ALIGN(4);
    } > RAM  AT> FLASH

    . = ALIGN(4);
    .ram_exec : {
        *(.ram_exec)
        __data_end__ = .;
    } > RAM AT> FLASH
    
   .bss  : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

  ._user_heap_stack :
  {
    . = ALIGN(4);
    PROVIDE ( _end = . );
    . = . + _heap_size_min;
    . = ALIGN(4);
  } > RAM

   /* __StackTop = 0x20040000; /* highest stack pointer is at the very end of the regular ram block */
 }

