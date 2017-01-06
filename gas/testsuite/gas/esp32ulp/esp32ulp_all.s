	 .data
foodata:.word 42
	.text
	.global a1
	.global a2
	.global a3
	.global a4
	.global __start
	.global l1
	.global l2
	.global l3
	.set c1, 0x03
	.set c2, 0x2222

__start:
        nop
// Add
        ADD R1, R2, R3                          //R1 = R2 + R3
        Add R1, R2, 0x1234                      //R1 = R2 + 0x1234

        .set value1, 0x03                       //constant value1=0x03
        Add R1, R2, value1                      //R1 = R2 + value1

        .global label1                          //declaration of variable label
        Add R1, R2, label1                      //R1 = R2 + label
label1: nop                                     //definition of variable label
// Sub
         SUB R1, R2, R3                         //R1 = R2 - R3

         sub R1, R2, 0x1234                     //R1 = R2 - 0x1234

         SUB R1, R2, value1                     //R1 = R2 - value1
         .global label2                         //declaration of variable label
         SUB R1, R2, label2                     //R1 = R2 - label
label2:  nop                                    //definition of variable label
// And
         AND R1, R2, R3                         //R1 = R2 & R3
         AND R1, R2, 0x1234                     //R1 = R2 & 0x1234
         AND R1, R2, value1                     //R1 = R2 & value1
         .global label                          //declaration of variable label
         AND R1, R2, label3                     //R1 = R2 & label
label3:  nop                                    //definition of variable label
// Or
       OR R1, R2, R3                            //R1 = R2 \| R3
       OR R1, R2, 0x1234                        //R1 = R2 \| 0x1234
       OR R1, R2, value1                        //R1 = R2 \| value1
       .global label4                           //declaration of variable label
       OR R1, R2, label4                        //R1 = R2 \|label
label4: nop                                     //definition of variable label
// LSH
       LSH R1, R2, R3                           //R1 = R2 << R3

       LSH R1, R2, 0x03                         //R1 = R2 << 0x03

       LSH R1, R2, value1                       //R1 = R2 << value1

       .global label5                           //declaration of variable label
       LSH R1, R2, label5                       //R1 = R2 << label
label5:  nop                                    //definition of variable label
// RSH
        RSH R1, R2, R3                          //R1 = R2 >> R3

        RSH R1, R2, 0x03                        //R1 = R2 >> 0x03
                                                                       
        RSH R1, R2, value1                      //R1 = R2 >> value1

        .global label6                          //declaration of variable label
        RSH R1, R2, label6                      //R1 = R2 >> label
label6:  nop                                    //definition of variable label
// Move
        MOVE       R1, R2                       //R1 = R2 >> R3

        MOVE       R1, 0x03                     //R1 = R2 >> 0x03

        MOVE       R1, value1                   //R1 = value1

        .global     label7                      //declaration of variable label
        MOVE        R1, label7                  //R1 = label
label7:  nop                                    //definition of variable label
// ST
        ST  R1, R2, 0x12                        //MEM[R2+0x12] = R1

        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
        .set      offs, 0x04                    // Define constant offs
        .text                                   //Text section definition
        MOVE      R1, 1                         // R1 = 1
        MOVE      R2, Addr1                     // R2 = Addr1
        ST        R1, R2, offs                  // MEM[R2 +  4] = R1
                                                // MEM[Addr1+4] will be 1
// After operation value at Addr1 will be = 1

        LD  R1, R2, 0x12                        //R1 = MEM[R2+0x12]

        .data                                   //Data section definition
Addr2:  .word     123                           // Define label Addr1 16 bit
        .set      offs, 0x04                    // Define constant offs
        .text                                   //Text section definition
        MOVE      R1, 1                         // R1 = 1
        MOVE      R2, Addr2                     // R2 = Addr1
        LD        R1, R2, offs                  // R1 = MEM[R2 +  4]
                                                // R1 will be 123
// After operation value in register R1 will be the same as in Addr1, i.e. 1
  // Jump   
        .global    j1
        Move       R1, j1
        JUMP       R1                           //Jumps to R1 pointer
        nop;
j1:     JUMP       0x120, EQ                    //Jumps to address 0x120 if ALU equivale to 0

        .global    labelj                       //declaration of variable label

        JUMP       labelj                       // jump to address - label
        nop
labelj:  nop                                    //definition of variable label
// Jumpr

pos:    JUMPR       4, 20, GE                   //Jump to address "position + 4" if value in R0 >= 20
        nop
        .global     labeljr                     //declaration of variable label
// Just making loop with 16 times NOP operation
        MOVE        R0, 16                      // load 16 to R0
        MOVE        R1, 1                       // load 1 to R1
labeljr:  SUB         R0, R0, R1                // R0 = R0-1;
        NOP                                     // do something
        JUMPR       labeljr,1,GE                // jump to address label if R0 >= 0

//jumps

         JUMPS     4, 20, EQ                    //Jump to address “position + 16” if value in Stage_Cnt == 20
         nop
         nop
        .global    labeljs                      //declaration of variable label
// Just making loop with 16 times NOP operation
        STAGE_RST                               // set stage_cnt to 0
labeljs:  STAGE_INC  1                          // stage_cnt++;
        NOP                                     // do something
        JUMPS       labeljs,16,LT               // jump to address label if stage_cnt < 16
        nop
// Stage count
        STAGE_RST
// Stage Inc
        STAGE_INC      10                       // stage_cnt += 10;

        .global    labelsi                      //declaration of variable label
// Just making loop with 16 times NOP operation
        STAGE_RST                               // set stage_cnt to 0
labelsi:  STAGE_INC  1                          // stage_cnt++;
        NOP                                     // do something
        JUMPS       labelsi,16,LT               // jump to address label if stage_cnt < 16
// Stage Dec

        STAGE_DEC      10                       // stage_cnt -= 10;

        .global    labelsd                      //declaration of variable label
// Just making loop with 16 times NOP operation
        STAGE_RST                               // set stage_cnt to 0
        STAGE_INC  16                           // set stage_cnt to 16
labelsd:  STAGE_DEC  1                          // stage_cnt--;
        NOP                                     // do something
        JUMPS       labelsd,0,GT                // jump to address label if stage_cnt > 0

// Wake
        .set wake_debug, 1                      // set global constant
        WAKE     wake_debug                     // Wakeup the chip

// Sleep
        .set sleep_cnt, 10                      // set global constant
        SLEEP  sleep_cnt                        // Sleep for 10 cycles
// Wait
        .set  wait_cnt, 10                      // set global constant
        WAIT  wait_cnt                          // wait for 10 cycles

// TSENS
        TSENS     R1, 10, 10                    // Measure temperature sensor for 10 cycles,
                                                // move result to R1 and wait 10 cycles
// ADC
       ADC      R1, 0, 1, 100                   // Measure value form ADC0 pad 2,
                                                //for 10 cycles and move result to R1
// REG_RD/REG_WR

        REG_RD      0x120, 1, 2                 // R0[1:2] = REG[0x120]
        REG_WR      0x120, 1, 2,0x10            // REG[0x120] = 0x10[1:2]

        HALT                                    // Move chip to powerdown
