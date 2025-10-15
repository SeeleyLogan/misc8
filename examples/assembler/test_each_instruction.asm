; test_each_instruction.asm

.main
    nop
    load    0xBA
    copyab          ; 0xBA  is in RB
    loadj   <var_a> ; <var_a> is in RJ
    fetch           ; 0xFF  is in RA
    add             ; 0xB9  is in RA, CF set (0xFF + 0xBA)
    loadj   <var_b>
    fetchj          ; <block_1> is in RJ, RA is undefined
    jz              ; Does nothing (ZF false)
    jc              ; Jumps to <block_1> (CF set)

        $block_2
    loadj   <var_d>
    load    0x42
    write           ; 0x42  is in memory at *<var_d>
    loadj   <var_e>
    fetchj          ; <block_3> is in RJ, RA is undefined
    jmp             ; Jumps to <block_3>

        $block_1
    copyba          ; 0xBA  is in RA
    sub             ; 0x00  is in RA, ZF set, CF unset (0xBA - 0xBA)
    nop
    loadj   <var_c>
    fetchj          ; <block_2> is in RJ, RA is undefined
    jz              ; Jumps to <block_2> (ZF set)

        $block_3
    nop
    nop
    halt
.end_main

.data
        $var_a
    0xFF

        $var_b
    <block_1>

        $var_c
    <block_2>

        $var_d
    0x00            ; a place to write to
        
        $var_e
    <block_3>
.end_data
