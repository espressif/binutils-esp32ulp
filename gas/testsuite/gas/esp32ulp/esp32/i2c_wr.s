	.macro nops
	sleep 1
	sleep 2
	sleep 3
	sleep 4
	sleep 5
	sleep 6
	sleep 7
	sleep 8
	.endm



	.text
    .set loop1_thres1, 20
    .global loop1_thres1
    .set ttt, 4
    

    .set i2c_addr, 0x77
    .set i2c_data1, 0x40
    .set i2c_high, 0x00
    .set i2c_low,  0x01
    .set i2c_sel,  0x00
    .set reg_addr,  0x18

    //.global i2c_addr
    //.global i2c_data1
    .global i2c_sel
    .global i2c_low
    .global i2c_high
    //.global reg_addr

start:
	I2C_RD  0xff,       0,0,1
	I2C_WR  0xff, 0x77, 0,0,2

    REG_RD  reg_addr, 0, 1

	I2C_RD  i2c_addr,       0,0,1
	I2C_WR  i2c_addr, i2c_data1, i2c_high,i2c_low,i2c_sel
	nop
    ADC      R1, 0, 1, 100                   // Measure value form ADC0 pad 2,
    ADC      R1, 0, 1                   // Measure value form ADC0 pad 2,
    nop

