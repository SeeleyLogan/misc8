.main
{
    load    64
    copyab
    load    3

    loadj   <b>
    write

    copyba
    loadj   <a>
    write

    load    0
    loadj   <out>
    write

    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

    $loop
    load    1
    copyab
    loadj   <b>
    fetch
    sub
    write

    loadj   <end>
    jz

    loadj   <out>
    fetch
    copyab
    loadj   <a>
    fetch
    add
    loadj   <out>
    write

    loadj   <loop>
    jmp

    $end
    loadj   <out>
    fetch
    halt
}

.data
{
        $a:     0x50
        $b:     0x51
        $out:   0x52
}

