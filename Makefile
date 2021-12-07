

PROJECT=minimal_pico
CC=arm-none-eabi-gcc
OBJCPY=arm-none-eabi-objcopy
ELF2UF2=./tools/elf2uf2
OPT=-O3
PAD_CKECKSUM=./tools/pad_checksum
CARGS=-fno-builtin -g -DRP2040_FEATHER -mcpu=cortex-m0plus -mthumb -ffunction-sections -fdata-sections -I./Inc/RpiPico -I./Inc
LARGS=-g -Xlinker --gc-sections -Xlinker -print-memory-usage -T ./minimal_pico.ld -Xlinker -Map="./out/$(PROJECT).map" 
LARGS_BS2=-nostdlib -T ./bs2_default.ld -Xlinker -Map="./out/bs2_default.map"
CPYARGS=-Obinary

all: clean_objs bs2_code_size main_uf2 

clean_objs:
	@rm -f ./out/*

clean: clean_objs
	@rm -f ./tools/elf2uf2
	@rm -f ./tools/pioasm

# compile tools (elf2uf2)
elf2uf2_tool:
	@g++ ./tools/src/main.cpp -o ./tools/elf2uf2

# boot stage code variants for the rp2040 feather
bs2_std.o:
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_std.S -o ./out/bs2_std.o

bs2_dspi.o:
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_dspi.S -o ./out/bs2_dspi.o

bs2_fast_qspi.o:
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_fast_qspi.S -o ./out/bs2_fast_qspi.o


# generating the boot stage2 assembly file
# via a full roundtrip Assembly -> .o (declared above) -> .elf -> .bin -> Assembly (data declaration with crc32 checksum) 
bs2_code.elf: bs2_std.o bs2_fast_qspi.o bs2_dspi.o
	@$(CC) $(LARGS_BS2) -o ./out/bs2_code.elf ./out/bs2_fast_qspi.o

bs2_code.bin: bs2_code.elf
	@$(OBJCPY) $(CPYARGS) ./out/bs2_code.elf ./out/bs2_code.bin

bs2_code_size: bs2_code.bin
	@echo '**********************************'
	@echo '* Boot Stage 2 Code is' `ls -l ./out/bs2_code.bin | cut -d ' ' -f5` 'bytes' 
	@echo '**********************************'

bootstage2.S: bs2_code.bin
	@$(PAD_CKECKSUM) -s 0xffffffff ./out/bs2_code.bin ./out/bootstage2.S

# rp2040 feather startup stage
pico_startup2.o: 
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_startup2.S -o ./out/pico_startup2.o 

bootstage2.o: bootstage2.S
	@$(CC) $(CARGS) $(OPT) -c ./out/bootstage2.S -o ./out/bootstage2.o 

minimal_main.o:
	@$(CC) $(CARGS) $(OPT) -c ./Src/rp2040/minimal_main.c -o ./out/minimal_main.o

# common libs
taskManager.o:
	@$(CC) $(CARGS) $(OPT) -c ./Src/common/taskManager.c -o ./out/taskManager.o

# main linking and generating flashable content
main_elf: bootstage2.o pico_startup2.o minimal_main.o bs2_fast_qspi.o
	$(CC) $(LARGS) -o ./out/$(PROJECT).elf ./out/minimal_main.o ./out/bootstage2.o ./out/pico_startup2.o ./out/bs2_fast_qspi.o

main_bin: main_elf
	@$(OBJCPY) $(CPYARGS) ./out/$(PROJECT).elf ./out/$(PROJECT).bin

main_uf2: elf2uf2_tool main_elf 
	$(ELF2UF2) -v ./out/$(PROJECT).elf ./out/$(PROJECT).uf2
