        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit

        .global __start
__start:
        
        .text                                   //Text section definition    	
// Register operations - relative operations
        Add R1, R2, R3
        Sub R1, R2, R3
        And R1, R2, R3
        Or  R1, R2, R3
        RSH R1, R2, R3
        LSH R1, R2, R3
        MOVE R1, R2

        .set   im_data_d, 0xff
        .set   im_data_r, 0xff
        .global im_data_r

        Add R1, R2, 0xff
        Add R1, R2, im_data_d
        Add R1, R2, im_data_r
        Sub R1, R2, 0xff
        Sub R1, R2, im_data_d
        Sub R1, R2, im_data_r
        And R1, R2, 0xff
        And R1, R2, im_data_d
        And R1, R2, im_data_r
        Or R1, R2, 0xff
        Or R1, R2, im_data_d
        Or R1, R2, im_data_r
        RSH R1, R2, 0xff
        RSH R1, R2, im_data_d
        RSH R1, R2, im_data_r
        LSH R1, R2, 0xff
        LSH R1, R2, im_data_d
        LSH R1, R2, im_data_r
        MOVE R1, 0xff
        MOVE R1, im_data_d
        MOVE R1, im_data_r

        STAGE_RST
        STAGE_INC   0xff
        STAGE_INC   im_data_d
        STAGE_INC   im_data_r
        STAGE_DEC   0xff
        STAGE_DEC   im_data_d
        STAGE_DEC   im_data_r



