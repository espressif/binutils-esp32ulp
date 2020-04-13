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
    
    .set  min_reg_Addr, 0 - 0
    .set  max_reg_Addr, 0x3ff + 0
    .set  min_reg_High, 0 - 0
    .set  max_reg_High, 31 + 0
    .set  min_reg_Low, 0 - 0
    .set  max_reg_Low, 31 + 0
    .set  min_reg_data, 0 - 0
    .set  max_reg_data, 255 + 0

	REG_RD  0, 0, 0 	
	REG_RD  0x3ff, 31, 31 	
	REG_WR  0, 0, 0, 0 	
	REG_WR  0x3ff, 31, 31, 255 	

	REG_RD  min_reg_Addr, min_reg_High, min_reg_Low 	
	REG_RD  max_reg_Addr, max_reg_High, max_reg_Low
	REG_WR  min_reg_Addr, min_reg_High, min_reg_Low, min_reg_data
	REG_WR  max_reg_Addr, max_reg_High, max_reg_Low, max_reg_data

	.global  min_reg_Addr2
	.global  max_reg_Addr2
	.global  min_reg_High2
	.global  max_reg_High2
	.global  min_reg_Low2
	.global  max_reg_Low2
	.global  min_reg_data2
	.global  max_reg_data2
    .set  min_reg_Addr2, 0 - 0
    .set  max_reg_Addr2, 0x3ff + 0
    .set  min_reg_High2, 0 - 0
    .set  max_reg_High2, 31 + 0
    .set  min_reg_Low2, 0 - 0
    .set  max_reg_Low2, 31 + 0
    .set  min_reg_data2, 0 - 0
    .set  max_reg_data2, 255 + 0

	REG_RD  min_reg_Addr2, min_reg_High2, min_reg_Low2 	
	REG_RD  max_reg_Addr2, max_reg_High2, max_reg_Low2 
	REG_WR  min_reg_Addr2, min_reg_High2, min_reg_Low2, min_reg_data2
	REG_WR  max_reg_Addr2, max_reg_High2, max_reg_Low2, max_reg_data2
