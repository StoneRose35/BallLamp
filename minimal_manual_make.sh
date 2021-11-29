#!/bin/bash


# preprocess bootloader
# compile and link
# generate bin-file
# pad and generate crc32
# reconvert to assembly file
arm-none-eabi-gcc -DRPI_PICO -mcpu=cortex-m0plus -nostdlib  -mthumb -O3 -ffunction-sections -fdata-sections -c ./Startup/pico_bs2_code.S -o ./out/bs2_code.o
arm-none-eabi-gcc -nostdlib -T ./bs2_default.ld -Xlinker -Map="./out/bs2_default.map" -o ./out/bs2_code.elf ./out/bs2_code.o
arm-none-eabi-objcopy -Obinary ./out/bs2_code.elf ./out/bs2_code.bin
./pad_checksum ./out/bs2_code.bin ./out/bootstage2.S


# compile
arm-none-eabi-gcc -nostdlib -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -c ./Startup/pico_startup2.S -o ./out/pico_startup2.o
arm-none-eabi-gcc -nostdlib -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -c ./out/bootstage2.S -o ./out/bootstage2.o
arm-none-eabi-gcc -nostdlib -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -c ./Src/minimal_main.c -o ./out/minimal_main.o


#deassemble
arm-none-eabi-gcc -nostdlib -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -S ./Src/minimal_main.c -o ./out/minimal_main.S

# link 
arm-none-eabi-gcc -nostdlib -Xlinker --gc-sections -Xlinker -print-memory-usage -T ./minimal_pico.ld -Xlinker -Map="./out/minimal_pico.map" -o ./out/minimal_pico.elf ./out/pico_startup2.o ./out/minimal_main.o ./out/bootstage2.o


# generate binary
arm-none-eabi-objcopy -Obinary ./out/minimal_pico.elf minimal_pico.bin

# convert to uf2
./elf2uf2 ./out/minimal_pico.elf ./out/minimal_pico.uf2
