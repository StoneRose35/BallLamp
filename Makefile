

PROJECT=pico_lamp
CC=arm-none-eabi-gcc
OBJCPY=arm-none-eabi-objcopy
ELF2UF2=./tools/elf2uf2
OPT=-Og
PAD_CKECKSUM=./tools/pad_checksum
CARGS=-fno-builtin -g -DRP2040_FEATHER -mcpu=cortex-m0plus -mthumb -ffunction-sections -fdata-sections -std=gnu11 -I./Inc/RpiPico -I./Inc -I./Inc/gen
LARGS=-g -Xlinker -print-memory-usage -mcpu=cortex-m0plus -mthumb -T./minimal_pico.ld -Xlinker -Map="./out/$(PROJECT).map" --specs=nosys.specs -Xlinker --gc-sections -static --specs=nano.specs
LARGS_BS2=-nostdlib -T ./bs2_default.ld -Xlinker -Map="./out/bs2_default.map"
CPYARGS=-Obinary
BOOTLOADER=bs2_fast_qspi

all: clean_objs bs2_code_size $(PROJECT).uf2 

RP2040_OBJS := $(patsubst Src/rp2040/%.c,out/%.o,$(wildcard Src/rp2040/*.c))
COMMON_OBJS := $(patsubst Src/common/%.c,out/%.o,$(wildcard Src/common/*.c))

all_rp2040: $(RP2040_OBJS) 

all_common: $(COMMON_OBJS)

clean_objs:
	@rm -f ./out/*
	@rm -f ./Inc/gen/*

clean: clean_objs
	@rm -f ./tools/elf2uf2
	@rm -f ./tools/pioasm

# compile tools (elf2uf2)
tools/elf2uf2:
	g++ ./tools/elf2uf2_src/main.cpp -o ./tools/elf2uf2

PIOASM_SRC_DIR := tools/pioasm_src/
PIOASM_GEN_DIR := $(PIOASM_SRC_DIR)gen/

tools/pioasm_src/gen:
	mkdir ./tools/pioasm_src/gen

Inc/gen:
	mkdir ./Inc/gen

tools/pioasm_src/gen/lexer.cpp: tools/pioasm_src/gen
	flex -o ./tools/pioasm_src/gen/lexer.cpp ./tools/pioasm_src/lexer.ll 

tools/pioasm_src/gen/parser.cpp: tools/pioasm_src/gen tools/pioasm_src/gen/lexer.cpp
	bison --defines=./tools/pioasm_src/gen/parser.hpp -o ./tools/pioasm_src/gen/parser.cpp ./tools/pioasm_src/parser.yy

tools/pioasm: tools/pioasm_src/gen/parser.cpp tools/pioasm_src/gen/lexer.cpp
	g++ -DYY_NO_UNISTD_H -Itools/pioasm_src -Itools/pioasm_src/gen -Wno-psabi -o ./tools/pioasm ./tools/pioasm_src/*.cpp ./tools/pioasm_src/gen/*.cpp 


# boot stage code variants for the rp2040 feather
bs2_std.o:
	$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_std.S -o ./out/bs2_std.o

bs2_dspi.o:
	$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_dspi.S -o ./out/bs2_dspi.o

bs2_fast_qspi.o:
	$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_fast_qspi.S -o ./out/bs2_fast_qspi.o


# generating the boot stage2 assembly file
# via a full roundtrip Assembly -> .o (declared above) -> .elf -> .bin -> Assembly (data declaration with crc32 checksum) 
bs2_code.elf: $(BOOTLOADER).o
	$(CC) $(LARGS_BS2) -o ./out/bs2_code.elf ./out/$(BOOTLOADER).o

bs2_code.bin: bs2_code.elf
	$(OBJCPY) $(CPYARGS) ./out/bs2_code.elf ./out/bs2_code.bin

bs2_code_size: bs2_code.bin
	@echo '**********************************'
	@echo '* Boot Stage 2 Code is' `ls -l ./out/bs2_code.bin | cut -d ' ' -f5` 'bytes' 
	@echo '**********************************'

bootstage2.S: bs2_code.bin
	$(PAD_CKECKSUM) -s 0xffffffff ./out/bs2_code.bin ./out/bootstage2.S

# rp2040 feather startup stage
pico_startup2.o: 
	$(CC) $(CARGS) $(OPT) -c ./Startup/pico_startup2.S -o ./out/pico_startup2.o 

bootstage2.o: bootstage2.S
	$(CC) $(CARGS) $(OPT) -c ./out/bootstage2.S -o ./out/bootstage2.o 

# common libs
out/%.o: Src/common/%.c
	$(CC) $(CARGS) $(OPT) -c $^ -o $@

# rp2040 specific libs
out/%.o: Src/rp2040/%.c
	$(CC) $(CARGS) $(OPT) -c $^ -o $@

Src/rp2040/neopixelDriver.c: Inc/gen/pioprogram.h

Src/rp2040/simple_neopixel.c: Inc/gen/pioprogram.h

# pio assembler
Inc/gen/pioprogram.h: Inc/gen tools/pioasm
	./tools/pioasm -o c-sdk ./Src/rp2040/ws2812.pio ./Inc/gen/pioprogram.h

# main linking and generating flashable content
$(PROJECT).elf: bootstage2.o pico_startup2.o all_rp2040 all_common
	$(CC) $(LARGS) -o ./out/$(PROJECT).elf ./out/*.o

$(PROJECT).bin: $(PROJECT).elf
	@$(OBJCPY) $(CPYARGS) ./out/$(PROJECT).elf ./out/$(PROJECT).bin

$(PROJECT).uf2: tools/elf2uf2 $(PROJECT).elf 
	$(ELF2UF2) -v ./out/$(PROJECT).elf ./out/$(PROJECT).uf2

# experimental stuff

testfunction.S: 
	$(CC) $(CARGS) $(OPT) -S ./Src/rp2040/testfunction.c -o testfunction.S