#include <stdint.h>
#include "romfunc.h"

volatile f_proc_fct floatAdd;
volatile f_proc_fct floatSub;
volatile f_proc_fct floatMult;
volatile f_proc_fct floatDiv;
volatile f_proc_fct_conv uint32ToFoat;

void * getRomFunction(char c1,char c2)
{
    void * fct;
    uint32_t code = (c1 << 8) + c2;
    //multicast (uint32_t -> void pointer -> function pointer/uint16_t pointer) here only to avoid compiler warnings
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)(void*)(uint32_t)(*((uint16_t*)0x18));
    fct = rom_table_lookup((uint16_t*)(void*)(uint32_t)*((uint16_t*)0x14),code);
    return fct;
}


void * getRomData(char c1,char c2)
{
    void * fct;
    uint32_t code = (c1 << 8) + c2;
    //multicast (uint32_t -> void pointer -> function pointer/uint16_t pointer) here only to avoid compiler warnings
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)(void*)(uint32_t)(*((uint16_t*)0x18));
    fct = rom_table_lookup((uint16_t*)(void*)(uint32_t)*((uint16_t*)0x16),code);
    return fct;
}


void flash_range_erase(uint32_t addr, uint32_t count, uint32_t block_size, uint8_t block_cmd)
{
    flash_range_erase_fn flash_range_erase_ptr = getRomFunction('R','E');
    flash_range_erase_ptr(addr,count,block_size,block_cmd);
}

void flash_range_program(uint32_t addr,const uint8_t * data, uint32_t cnt)
{
    flash_range_program_fn flash_range_program_ptr = getRomFunction('R','P');
    flash_range_program_ptr(addr,data,cnt);
}

float f_add(float a,float b)
{
    f_proc_fct floatFct = (f_proc_fct)((uint32_t)getRomData('S','F')+0);
    return floatFct(a,b);
}