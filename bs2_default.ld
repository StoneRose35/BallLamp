
MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 8192k  /* 8MB on rp2040 feather */
    RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
}

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
    
    .text : {
        *(.text*)
        . = ALIGN(4);
        __etext = .;
    }
    
    .data : {
    	__data_start__ = .;
    	*(.data*)
    	__data_end__ = .;
    	. = ALIGN(4);
    	
    	
    } > RAM AT> FLASH 
    
    .bss  : {
        . = ALIGN(4);
        __bss_start__ = .;
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.bss*)))
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM
    
    .head : {
    	__head_start__ = .;
    	. = . + 0x1000;
    }
    __StackTop = __data_end__ + 0x1000;
}
