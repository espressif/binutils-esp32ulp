
        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
        .set      offs, 0x20                    // Define constant offs

        .set      offs2, 0x20                    // Define constant offs
        .global   offs2


        .global __start
__start:
        
        .text                                   //Text section definition
        LD       R1, R2, 0x20                  
        LD       R1, R2, offs                   
        LD       R1, R2, offs2                   
        LDL       R1, R2, 0x20                  
        LDL       R1, R2, offs                   
        LDL       R1, R2, offs2                   
        LDH       R1, R2, 0x20                  
        LDH       R1, R2, offs                   
        LDH       R1, R2, offs2                   
