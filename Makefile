

PROJECT=minimal_pico
CC=arm-none-eabi-gcc
OBJCPY=arm-none-eabi-objcopy
ELF2UF2=./tools/elf2uf2
OPT=-O3
PAD_CKECKSUM=./tools/pad_checksum
CARGS=-nostdlib -g -DRPI_PICO -mcpu=cortex-m0plus -mthumb -ffunction-sections -fdata-sections -I./Inc/RpiPico
LARGS=-nostdlib -g -Xlinker --gc-sections -Xlinker -print-memory-usage -T ./minimal_pico.ld -Xlinker -Map="./out/$(PROJECT).map" 
LARGS_BS2=-nostdlib -T ./bs2_default.ld -Xlinker -Map="./out/bs2_default.map"
CPYARGS=-Obinary

all: clean bs2_code_size main_uf2 

clean:
	@rm -f ./out/*

bs2_std.o:
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_std.S -o ./out/bs2_std.o

bs2_dspi.o:
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_dspi.S -o ./out/bs2_dspi.o

bs2_fast_qspi.o:
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_bs2_fast_qspi.S -o ./out/bs2_fast_qspi.o

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

pico_startup2.o: 
	@$(CC) $(CARGS) $(OPT) -c ./Startup/pico_startup2.S -o ./out/pico_startup2.o 

bootstage2.o: bootstage2.S
	@$(CC) $(CARGS) $(OPT) -c ./out/bootstage2.S -o ./out/bootstage2.o 

minimal_main.o:
	@$(CC) $(CARGS) $(OPT) -c ./Src/minimal_main.c -o ./out/minimal_main.o

deassemble: 
	$(CC) $(CARGS) $(OPT) -S ./Src/minimal_main.c -o ./out/minimal_main.S

main_elf: bootstage2.o pico_startup2.o minimal_main.o bs2_fast_qspi.o
	$(CC) $(LARGS) -o ./out/$(PROJECT).elf ./out/minimal_main.o ./out/bootstage2.o ./out/pico_startup2.o ./out/bs2_fast_qspi.o

main_bin: main_elf
	@$(OBJCPY) $(CPYARGS) ./out/$(PROJECT).elf ./out/$(PROJECT).bin

main_uf2: main_elf
	$(ELF2UF2) -v ./out/$(PROJECT).elf ./out/$(PROJECT).uf2
