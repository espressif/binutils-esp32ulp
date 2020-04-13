        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit


        .global __start
__start:
        
        .text
        .global jmp2         
        nop
        wake
        halt
        