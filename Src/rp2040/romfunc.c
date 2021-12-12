#include "types.h"
#include "romfunc.h"



void * getRomFunction(char c1,char c2)
{
    void * fct;
    uint32_t code = (c1 << 8) + c2;
    //uint32_t* func_table_address = (uint32_t*)*((uint16_t*)0x14);
    rom_table_lookup_fn rom_table_lookup = (rom_table_lookup_fn)0x18;
    fct = rom_table_lookup((uint16_t*)0x14,code);
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