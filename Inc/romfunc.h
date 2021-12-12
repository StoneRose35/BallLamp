
#ifndef __ROM_FUNC
#define __ROM_FUNC

#include "types.h"

typedef void *(*rom_table_lookup_fn)(uint16_t *table, uint32_t code);
typedef void (*flash_range_erase_fn)(uint32_t addr,uint32_t count,uint32_t block_size,uint8_t block_cmd);
void flash_range_erase(uint32_t addr, uint32_t count, uint32_t block_size, uint8_t block_cmd);

typedef void (*flash_range_program_fn)(uint32_t addr, const uint8_t *data, uint32_t count);
void flash_range_program(uint32_t addr, const uint8_t *data, uint32_t count);

#endif