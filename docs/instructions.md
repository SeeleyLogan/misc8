# MISC8 Instructions

## Overview

Programming the MISC8 requires a firm understanding of its ISA.

This page will take a top-down approach to understanding how the computer processes and executes instructions.

There will also be documentation on each of the **16 instructions** and their implementation.

## Instruction Listings

| Instruction Name   | Opcode   | Cycles   | Description                               |
|--------------------|----------|----------|-------------------------------------------|
| `nop`              | `0x00`   | `5`      | No operation                              |
| `add`              | `0x01`   | `6`      | `RA += RB`, Sets flags                    |
| `sub`              | `0x02`   | `6`      | `RA -= RB`, Sets flags*                   |
| `load`             | `0x03`   | `8`      | `RA =  [next byte in memory]`**           |
| `loadj`            | `0x04`   | `11`     | `RJ =  [next 2 bytes in memory]`**        |
| `fetch`            | `0x05`   | `8`      | `RA =  [byte at address in RJ]`           |
| `fetchj`           | `0x06`   | `18`     | `RJ =  [2 bytes at address in RJ]`        |
| `write`            | `0x07`   | `8`      | `[byte at address in RJ] = RA`            |
| `copyab`           | `0x08`   | `6`      | `RB =  RA`                                |
| `copyba`           | `0x09`   | `6`      | `RA =  RB`                                |
| `jmp`              | `0x0A`   | `7`      | Jumps to instruction at address in `RJ`   |
| `jc`               | `0x0B`   | `7\|5`   | Jumps to address in `RJ` if `CF` is set   |
| `jz`               | `0x0C`   | `7\|5`   | Jumps to address in `RJ` if `ZF` is set   |
| `dwritei`          | `0x0D`   | `6`      | Writes `RA` to LCD in instruction mode*** |
| `dwrited`          | `0x0E`   | `6`      | Writes `RA` to LCD in data mode***        |
| `halt`             | `0x0F`   | `32`     | Stops the clock                           |

\* `sub` works by taking the 2's complement of `RB`, and adding it to `RA`. Which will result in `CF` being set if `RA >= RB`.

\*\* `load` and `loadj` both load the bytes following the instruction into their corresponding register (see examples).

\*\*\* The **1602A LCD** has timings slower than what the `dwritei` and `dwrited` instruction can provide. Calling a `delay` function before writing each byte to the display is required.
