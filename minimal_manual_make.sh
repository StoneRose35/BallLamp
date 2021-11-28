#!/bin/bash

# compile
arm-none-eabi-gcc -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -c ./Startup/pico_startup_minimal.S -o ./out/pico_startup_minimal.o
arm-none-eabi-gcc -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -c ./Startup/bs2_default_padded_checksummed.S -o ./out/bs2_default_padded_checksummed.o
arm-none-eabi-gcc -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -c ./Src/minimal_main.c -o ./out/minimal_main.o



# link 
arm-none-eabi-gcc -Xlinker --gc-sections -Xlinker -print-memory-usage -T ./minimal_pico.ld -Xlinker -Map="./out/minimal_pico.map" -o ./out/minimal_pico.elf ./out/pico_startup_minimal.o ./out/minimal_main.o ./out/bs2_default_padded_checksummed.o


# generate binary
 arm-none-eabi-objcopy -Obinary ./out/minimal_pico.elf minimal_pico.bin

./elf2uf2 ./out/minimal_pico.elf ./out/minimal_pico.uf2
