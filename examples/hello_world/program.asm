; test_each_instruction.asm

.main
    ; init
    load    x38
    dwritei
    load    x0C
    dwritei
    load    x06
    dwritei
    load    x01
    dwritei

    ; "hi"
    load    x48
    dwrited
    load    x45
    dwrited
    load    x4C
    dwrited
    load    x4C
    dwrited
    load    x4F
    dwrited
    load    x2C
    dwrited
    load    x20
    dwrited
    load    x57
    dwrited
    load    x4F
    dwrited
    load    x52
    dwrited
    load    x4C
    dwrited
    load    x44
    dwrited
    load    x21
    dwrited

    halt
.end_main

.data
    
.end_data
