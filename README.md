# MISC8: Minimum Instruction Set Computer

This project currently includes an emulator for my homebrew breadboard computer.

Future programs will include an assembler and a compiler.

I also plan to extend the emulator by adding an emulator for the 1602A LCD, this way `dwritei` and `dwrited` can be debugged.

## About the MISC8

The computer is based on Ben Eater's own homebrew. However, mine has a few improvements.

> 285kHz Clock Speed \
> 12-bit addresses \
> 4 KiB of programmable ROM \
> 2 programs (each 4 KiB) \
> 2 general purpose 8-bit registers \
> 1 address register \
> 16 instructions \
> 1602A LCD

More information can be found in `./docs`.

## Compiling

For a quick compile-all: simply run `make` and all binaries will be placed in `./bin`

If you want to only compile the emulator, `make emulator` will do the trick.

## Usage

### Emulator

After compiling, a binary with the name `misc8_emulator` should appear in the `./bin` folder. You can either call this binary directly or by using `make run-emulator`.

The emulator takes one argument: the program file.

The program file is where you write the MISC8 program to. It should be a binary file, not a text one.

I recommend looking at the `./examples/emulator/` to get an idea on how to create programs. There's also `./docs` you can read which will help as well.

You can pass a program file to the MISC8 by using the `-p` flag.

> Example: \
> `misc8_emulator -p./program.bin`

Or if you're using `make`

> Example: \
> `make run-emulator "ARGS=-p./program.bin`

## Examples

I try to demonstrate every feature of the MISC8 and its associated programs. I add these examples to `./examples`.

Feel free to examples of your own and contribute them back, I love seeing what other people make!