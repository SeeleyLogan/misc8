Logan Seeley

## Test Each Instruction

This program does what the name implies: it tests each instruction at least once.

Also: because powershell sucks, run this in command prompt OR add --% before arguments

Compile:\
`make run-assembler "ARGS=-i./examples/hello_world/program.asm -o./bin/hello_world.bin"`

Run:\
`make run-emulator "ARGS=-i./bin/hello_world.bin"`
