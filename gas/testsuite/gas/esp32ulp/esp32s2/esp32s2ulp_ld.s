
        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
        .set      offs, 0x20                    // Define constant offs

        .set      offs2, 0x20                   // Define constant offs
        .global   offs2

        .set      offs_min, -1024               // Define min constant offs
        .set      offs_max,  1023               // Define max constant offs
        .global   offs2_min
        .global   offs2_max
        .set      offs2_min, -1024               // Define min constant offs
        .set      offs2_max,  1023               // Define max constant offs


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
        LD       R1, R2, -1024
        LD       R1, R2, 1023
        LD       R1, R2, offs_min
        LD       R1, R2, offs_max
        LD       R1, R2, offs2_min
        LD       R1, R2, offs2_max
