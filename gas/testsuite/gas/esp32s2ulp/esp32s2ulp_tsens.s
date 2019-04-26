        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit

        .set wait_delay1, 0x04
        .set wait_delay2, 0x04
        .global wait_delay2

        .global __start
__start:
        
        .text
        .global jmp2         
        nop
        TSENS R1, 0x04        
        TSENS R1, wait_delay1        
        TSENS R1, wait_delay2
