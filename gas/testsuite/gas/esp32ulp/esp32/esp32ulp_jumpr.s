        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit

        .set  check_jump1, 0x10
        .set  check_jump2, 0x10
        .global check_jump2
        
        .set  check_thres1, 0x04
        .set  check_thres2, 0x04
        .global check_thres2
        

        .global __start
__start:
        
        .text
        .global jmp2         
        nop

// Relative Jump
	JUMPR 0x04, 0x4, LT
	JUMPR 0x04, 0x4, LE
	JUMPR 0x04, 0x4, EQ
	JUMPR 0x04, 0x4, GT
	JUMPR 0x04, 0x4, GE

	jumpr Check1, 0x05, GE
	jumpr 0x04, 0x04, GE
Check1:
	JUMPR 0x04, 0x4, EQ
        nop
        nop
	JUMPR 0x04, 0x4, LE
	JUMPR check_jump1, 0x4, LT
	JUMPR check_jump1, 0x4, EQ
	JUMPR check_jump1, 0x4, LE
	JUMPR 0x04, check_thres1, LE
check_jump3:
	JUMPR check_jump1, check_thres1, LE
	JUMPR check_jump2, check_thres1, LE	
	JUMPR check_jump2, check_thres2, LE	
	JUMPR check_jump3, check_thres2, GT	

	JUMPR check_jump3, check_thres2, EQ
	jumpr Check2, check_thres2, GE
	jumpr check_jump3, check_thres2, GE
Check2:
        nop
        nop
        nop
        nop



        
    
