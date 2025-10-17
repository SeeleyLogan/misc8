CC 	  = clang
CDBGC = gcc
DBG   = gdb

INCDIR = ./include
CFLAGS = -Wall -std=c99 -pedantic -I$(INCDIR) -D_GNU_SOURCE
SRCDIR = ./src
BINDIR = ./bin

EMULATOR_TARGET = $(BINDIR)/misc8_emulator
EMULATOR_SOURCE = $(SRCDIR)/emulator/main.c

assembler_sARGET = $(BINDIR)/misc8_assembler
ASSEMBLER_SOURCE = $(SRCDIR)/assembler/main.c



.PHONY: all emulator run-emulator assembler run-assembler clean

all: $(EMULATOR_TARGET) $(assembler_sARGET)



$(EMULATOR_TARGET): $(EMULATOR_SOURCE) $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@;printf "\nSize of EMULATOR_TARGET:\n";size $(EMULATOR_TARGET);echo

emulator: $(EMULATOR_TARGET)

run-emulator: emulator
	@$(EMULATOR_TARGET) $(ARGS); EXIT_CODE=$$?; printf "\n\nEXIT CODE: 0x%x\n" $$EXIT_CODE



$(assembler_sARGET): $(ASSEMBLER_SOURCE) $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@;printf "\nSize of assembler_sARGET:\n";size $(assembler_sARGET);echo

assembler: $(assembler_sARGET)

run-assembler: assembler
	@$(assembler_sARGET) $(ARGS); EXIT_CODE=$$?; printf "\n\nEXIT CODE: 0x%x\n" $$EXIT_CODE



$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR)
	$(MAKE) all
