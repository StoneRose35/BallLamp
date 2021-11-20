#!/bin/bash

# basic makefile for the pi pico


#compile
arm-none-eabi-gcc -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -std=gnu11 -o ./out/simplemain.o -c ./Src/simplemain.c
arm-none-eabi-gcc -DRPI_PICO -mcpu=cortex-m0plus  -mthumb -O3 -ffunction-sections -fdata-sections -I./rpi_sdk_excerpt -o ./out/crt0.o   -c ./Startup/crt0.S


#link
arm-none-eabi-g++  -mcpu=cortex-m0plus -mthumb -O3 -Wl,--build-id=none --specs=nosys.specs -Wl,--script=./memmap_default.ld -Wl,-z,max-page-size=4096 -Wl,--gc-sections ./out/simplemain.o ./out/crt0.o -o ./out/balllamp.elf  ./Startup/bs2_default_padded_checksummed.S 


# convert the output to the uf2 format
arm-none-eabi-objcopy -Oihex ./out/balllamp.elf balllamp.hex
arm-none-eabi-objcopy -Obinary ./out/balllamp.elf balllamp.bin
arm-none-eabi-objdump -h ./out/balllamp.elf >./out/balllamp.dis
arm-none-eabi-objdump -d ./out/balllamp.elf >>balllamp.dis
./elf2uf2/elf2uf2 ./out/blink.elf ./out/blink.uf2
