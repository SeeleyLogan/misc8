DEBUG ?= 0
INFO  ?= 0
OPT   ?= O0
STD   ?= c99
EXTRA ?= 

CC 	  = gcc

ifeq ($(OS), Windows_NT)
    SHARED_FLAGS = -shared
	SHARED_EXT   = .dll
	EXE_EXT		 = .exe
else ifeq ($(shell uname -s), Linux)
    SHARED_FLAGS = -shared -fPIC
	SHARED_EXT   = .so
	EXE_EXT		 =
endif

INCDIR = ./inc
CFLAGS = -I$(INCDIR) -Wall -Wextra -std=$(STD) -pedantic -D_GNU_SOURCE -$(OPT) $(EXTRA)
SRCDIR = ./src
BINDIR = ./bin

EMULATOR_TARGET = $(BINDIR)/misc8_emulator$(EXE_EXT)
EMULATOR_SOURCE = $(SRCDIR)/emulator/main.c

ASSEMBLER_TARGET = $(BINDIR)/misc8_assembler$(EXE_EXT)
ASSEMBLER_SOURCE = $(SRCDIR)/assembler/main.c

ifeq ($(DEBUG), 1)
	CFLAGS += -g -DDEBUG
endif

.PHONY: all run clean lib

all: $(EMULATOR_TARGET) $(ASSEMBLER_TARGET)

emulator: $(EMULATOR_TARGET)

$(EMULATOR_TARGET): $(EMULATOR_SOURCE) $(BINDIR)
	@printf "\nCompiling Main File\n"
	$(CC) $(CFLAGS) $< -o $@
	@printf "\nSize of target:\n"
	@size $(EMULATOR_TARGET)
	@echo

run-emulator: emulator
	$(EMULATOR_TARGET) $(ARGS)
	@EXIT_CODE=$$?
	@printf "\n\nEXIT CODE: 0x%x\n" $$EXIT_CODE

assembler: $(ASSEMBLER_TARGET)

$(ASSEMBLER_TARGET): $(ASSEMBLER_SOURCE) $(BINDIR)
	@printf "\nCompiling Main File\n"
	$(CC) $(CFLAGS) $< -o $@
	@printf "\nSize of target:\n"
	@size $(ASSEMBLER_TARGET)
	@echo

run-assembler: assembler
	$(ASSEMBLER_TARGET) $(ARGS)
	@EXIT_CODE=$$?
	@printf "\n\nEXIT CODE: 0x%x\n" $$EXIT_CODE

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -rf $(BINDIR)

