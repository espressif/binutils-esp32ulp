	.text
	.set min_add, -32767
	.global min_add1
	.global max_add2

	.set  min_wait1, 0
	.set  max_wait1, 65535
	.global  min_wait2
	.global  max_wait2

	.set  min_jump1, 0
	.set  max_jump1, 2047
	.global  min_jump2
	.global  max_jump2

	.set  min_jumpr1, -80- 0
	.set  max_jumpr1, 80 + 0
	.set  min_thresh1, -32767-0
	.set  max_thresh1, 32767 +0
	.global  min_jumpr2
	.global  max_jumpr2
	.global  min_thresh2
	.global  max_thresh2


	.set  min_threshs1, -256
	.set  max_threshs1, 255
	.global  min_threshs1
	.global  max_threshs1

	.set  min_wr_mem1, 0 - 0
	.set  max_wr_mem1, 2047 + 0
	//.global  min_wr_mem1
	//.global  max_wr_mem1
	.global  min_wr_mem2
	.global  max_wr_mem2


	.set  min_sleep1, 0 - 0
	.set  max_sleep1, 15 + 0
	.global  min_sleep2
	.global  max_sleep2

	.set  min_wake1, 0 - 0
	.set  max_wake1, 1 + 0

__start:
        nop
        Add R1, R2, min_add
        Add R1, R2, min_add1
        Add R1, R2, max_add2

        WAIT  min_wait1
        WAIT  max_wait1

          WAIT  min_wait2
          WAIT  max_wait2

	JUMP min_jump1	
	JUMP max_jump1

	JUMP min_jump2	
	JUMP max_jump2
		
	JUMPR min_jumpr1, min_thresh1, LT	
	JUMPR max_jumpr1, max_thresh1, LT

	JUMPR min_jumpr2, min_thresh2, LT	
	JUMPR max_jumpr2, max_thresh2, LT	

	JUMPS min_jumpr2, min_threshs3, EQ	
	JUMPS max_jumpr2, max_threshs3, EQ	

	JUMPS min_jumpr2, min_threshs3, EQ	
	JUMPS max_jumpr2, max_threshs3, EQ	


	ST   R1, R2, min_wr_mem1
	ST   R1, R2, max_wr_mem1
	ST   R1, R2, min_wr_mem2
	ST   R1, R2, max_wr_mem2
	
	LD   R1, R2, min_wr_mem1
	LD   R1, R2, max_wr_mem1
	LD   R1, R2, min_wr_mem2
	LD   R1, R2, max_wr_mem2

	SLEEP min_sleep1
	SLEEP max_sleep1
	SLEEP min_sleep2
	SLEEP max_sleep2

	WAKE	min_wake1
	WAKE	max_wake1	
	WAKE	min_wake2
	WAKE	max_wake2

	.set  min_tsens_meas1, 0 - 0
	.set  max_tsens_meas1, 4095 + 0
	.set  min_tsens_wait1, 0 - 0
	.set  max_tsens_wait1, 16383 + 0
	.global  min_tsens_meas2
	.global  max_tsens_meas2
	.global  min_tsens_wait2
	.global  max_tsens_wait2

	TSENS     R1, min_tsens_meas1, min_tsens_wait1
	TSENS     R1, max_tsens_meas1, max_tsens_wait1
	TSENS     R1, min_tsens_meas2, min_tsens_wait2
	TSENS     R1, max_tsens_meas2, max_tsens_wait2
	
	.set  min_adc_sel, 0 - 0  
	.set  max_adc_sel, 1 + 0  
	.set  min_adc_mux, 0 - 0  
	.set  max_adc_mux, 15 + 0  
	.set  min_adc_cyc, 0 - 0
	.set  max_adc_cyc, 65535 + 0
	.global  min_adc_sel2
	.global  max_adc_sel2
	.global  min_adc_mux2
	.global  max_adc_mux2
	.global  min_adc_cyc2
	.global  max_adc_cyc2
	
	ADC   R1, min_adc_sel, min_adc_mux, min_adc_cyc
	ADC   R1, max_adc_sel, max_adc_mux, max_adc_cyc

	ADC   R1, min_adc_sel2, min_adc_mux2, min_adc_cyc2
	ADC   R1, max_adc_sel2, max_adc_mux2, max_adc_cyc2

    .set  min_reg_Addr, 0 - 0
    .set  max_reg_Addr, 1023 + 0
    .set  min_reg_High, 0 - 0
    .set  max_reg_High, 31 + 0
    .set  min_reg_Low, 0 - 0
    .set  max_reg_Low, 31 + 0
    .set  min_reg_data, 0 - 0
    .set  max_reg_data, 255 + 0

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

	REG_RD  min_reg_Addr2, min_reg_High2, min_reg_Low2 	
	REG_RD  max_reg_Addr2, max_reg_High2, max_reg_Low2 
	REG_WR  min_reg_Addr2, min_reg_High2, min_reg_Low2, min_reg_data2
	REG_WR  max_reg_Addr2, max_reg_High2, max_reg_Low2, max_reg_data2
