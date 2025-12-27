Logan Seeley

## Test Each Instruction

This program does what the name implies: it tests each instruction at least once.

Also: because powershell sucks, run this in command prompt OR add --% before arguments

Compile:\
`make run-assembler "ARGS=-i./examples/test_each_instruction/program.asm -o./bin/test_each_instruction.bin"`

Run:\
`make run-emulator "ARGS=-i./bin/test_each_instruction.bin"`
