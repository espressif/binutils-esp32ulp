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
jmp1:
jmp2:   nop
        jump 0x04
        jump check_jump1
        jump check_jump2
        jump R1
        jump 0x04, EQ
        jump check_jump1, EQ        
        jump check_jump2, EQ
        jump R1, EQ
        jump 0x04, OV
        jump check_jump1, OV        
        jump check_jump2, OV
        jump R1, OV
// Jump to labele
        jump jmp1, EQ        
        jump jmp2, EQ
        jump jmp1, OV        
        jump jmp2, OV

// Relative Jump
	JUMPR 0x04, 0x4, LT
	JUMPR check_jump1, 0x4, LT
	JUMPR 0x04, check_thres1, LT
	JUMPR check_jump1, check_thres1, LT
	JUMPR check_jump2, check_thres1, LT	
	JUMPR check_jump2, check_thres2, LT	
	JUMPR 0x04, 0x4, GT
	JUMPR check_jump1, 0x4, GT
	JUMPR 0x04, check_thres1, GT
	JUMPR check_jump1, check_thres1, GT	
	JUMPR check_jump2, check_thres1, GT	
	JUMPR check_jump2, check_thres2, GT	
	JUMPR 0x04, 0x4, EQ
	JUMPR check_jump1, 0x4, EQ
	JUMPR 0x04, check_thres1, EQ
	JUMPR check_jump1, check_thres1, EQ	
	JUMPR check_jump2, check_thres1, EQ	
	JUMPR check_jump2, check_thres2, EQ

// Jumps commands...
    jumps 0x4, 0x04, EQ
    jumps 0x04, check_thres1, EQ
    jumps check_jump1, check_thres1, EQ
    jumps check_jump1, check_thres2, EQ
    jumps check_jump2, check_thres1, EQ
    jumps check_jump2, check_thres2, EQ

    jumps 0x0, 0x00, EQ
    jumps 0x0, 0x00, LT
    jumps 0x0, 0x00, GT
    jumps 0x0, 0x00, LE
    jumps 0x0, 0x00, GE

    
