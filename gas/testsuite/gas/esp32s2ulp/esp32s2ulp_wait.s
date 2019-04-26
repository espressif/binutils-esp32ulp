        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
        .set min_wait1, 0
        .set max_wait1, 0xffff

        .set min_wait2, 0x00
        .set max_wait2, 0xffff
        .global min_wait2
        .global max_wait2

        .global __start
__start:
        
        .text                                   //Text section definition
    
        WAIT  0
        WAIT  0xffff
        WAIT  min_wait1
        WAIT  max_wait1
        WAIT  min_wait2
        WAIT  max_wait2
