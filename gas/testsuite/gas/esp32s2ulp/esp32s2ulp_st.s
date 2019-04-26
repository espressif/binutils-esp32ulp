
        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
        .set      offs, 0x20                    // Define constant offs
        .set      lab1, 0x01                    // Define constant offs

        .set      offs2, 0x20                    // Define constant offs
        .set      lab2,  0x1                    // Define constant offs
        .global   lab2
        .global   offs2
        

        .global __start
__start:
        
        .text                                   //Text section definition
        STL       R1, R2, 0x20                  // 89210068
        STL       R1, R2, offs                  // 89210068
        STL       R1, R2, offs,  1              // 99200068
        STL       R1, R2, offs,  lab1           // 99200068
        STL       R1, R2, offs2, lab1           // 
        STL       R1, R2, offs,  lab2           // 
        STL       R1, R2, offs2, lab2           // 

        STH       R1, R2, 0x20                  // C9210068
        STH       R1, R2, offs                  // C9210068
        STH       R1, R2, offs,  1              // D9200068
        STH       R1, R2, offs,  lab1           // D9200068
        STH       R1, R2, offs2, lab1           // 
        STH       R1, R2, offs,  lab2           // 
        STH       R1, R2, offs2, lab2           // 
        NOP
        NOP

//      ST32  Rdst, Rsrc, offset, label                           
        ST32  R1, R2, 0x10, 1                  // 19100068
        ST32  R1, R2, offs, lab1               // 19200068
        ST32  R1, R2, offs, lab2               // 09200068
        ST32  R1, R2, offs2,lab2               // 09000068
//      STI   Rdst, Rsrc, label      
        STI32 R1, R2, 1                        // 19000062
        STI32 R1, R2, lab1                     // 19000062
        NOP
        STI   R1, R2                           // 89010062  
        STI   R1, R2, 1                        // 99000062 
        STI   R1, R2, lab1                     // 99000062 
        STI   R1, R2, lab2
        NOP
        STO   0x20
        STO   offs
        STO   offs2
        NOP
        NOP
        NOP
        NOP

// After operation value at Addr1 will be = 1