        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
        .set i2c_addr, 0x77
        .set i2c_data1, 0x40
        .set i2c_high, 0x00
        .set i2c_low,  0x01
        .set i2c_sel,  0x00
        .set test_val,  1

        .set i2c_addr2, 0x77
        .set i2c_data2, 0x40
        .set i2c_high2, 0x00
        .set i2c_low2,  0x01
        .set i2c_sel2,  0x00
        .global i2c_addr2
        .global i2c_data2
        .global i2c_high2
        .global i2c_low2
        .global i2c_sel2

        .global __start
__start:
        
        .text                                   //Text section definition
    
    	I2C_WR  0x77, 0x40, 0x00,0x01,0x00
    	I2C_WR  i2c_addr, i2c_data1, i2c_high,i2c_low,i2c_sel
    	I2C_WR  i2c_addr2, i2c_data2, i2c_high2,i2c_low2,i2c_sel2

    	I2C_RD  0x77,      0x00,0x01,0x00
    	I2C_RD  i2c_addr,  i2c_high,i2c_low,i2c_sel
    	I2C_RD  i2c_addr2, i2c_high2,i2c_low2,i2c_sel2
