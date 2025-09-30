CC 	  = clang
CDBGC = gcc
DBG   = gdb

CFLAGS = -Wall -std=c99 -pedantic -D_GNU_SOURCE
SRCDIR = ./src
BINDIR = ./bin

EMULATOR_TARGET = $(BINDIR)/misc8_emulator
EMULATOR_TARDBG = $(BINDIR)/misc8_emulator_dbg
EMULATOR_SOURCE = $(SRCDIR)/misc8_emulator/main.c

.PHONY: all emulator run-emulator clean emulator-debug run-emulator-debug

all: $(EMULATOR_TARGET)

$(EMULATOR_TARGET): $(EMULATOR_SOURCE) $(BINDIR)
	$(CC) $(CFLAGS) $< -o $@;printf "\nSize of EMULATOR_TARGET:\n";size $(EMULATOR_TARGET);echo

$(EMULATOR_TARDBG): $(EMULATOR_SOURCE) $(BINDIR)
	$(CDBGC) $(CFLAGS) $< -o $@ -g

emulator-debug: $(EMULATOR_TARDBG)

$(BINDIR):
	mkdir -p $(BINDIR)

emulator: $(EMULATOR_TARGET)

run-emulator: emulator
	@$(EMULATOR_TARGET) $(ARGS); EXIT_CODE=$$?; printf "\n\nEXIT CODE: 0x%x\n" $$EXIT_CODE

run-emulator-debug: emulator-debug
	@$(DBG) $(EMULATOR_TARDBG) $(ARGS); EXIT_CODE=$$?; printf "\n\nEXIT CODE: 0x%x\n" $$EXIT_CODE

clean:
	rm -rf $(BINDIR)
	$(MAKE) all
