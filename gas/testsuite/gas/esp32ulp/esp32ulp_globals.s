	.text
	.global min_add1
	.global max_add2

	.set min_add1, -32768
	.set max_add2,  32767

        .set  min_wait2, 0
	.set  max_wait2, 65535 +0
	.global  min_wait2
	.global  max_wait2

	.set  min_jump2, 0
	.set  max_jump2, 2047+0
	.global  min_jump2
	.global  max_jump2

	.set  min_jumpr2, -40- 0
	.set  max_jumpr2,  80 + 0
	.set  min_thresh2, -32768
	.set  max_thresh2, 32767
	.global  min_jumpr2
	.global  max_jumpr2
	.global  min_thresh2
	.global  max_thresh2

	.set  min_threshs3, -128-0
	.set  max_threshs3, 127+0
	.global  min_threshs3
	.global  max_threshs3


	.set  min_wr_mem2, 0 - 0
	.set  max_wr_mem2, 2047 + 0
	.global  min_wr_mem2
	.global  max_wr_mem2

	.set  min_sleep2, 0 - 0
	.set  max_sleep2, 15 + 0
	.global  min_sleep2
	.global  max_sleep2

	.set  min_wake2, 0 - 0
	.set  max_wake2, 1 + 0
	.global  min_wake2
	.global  max_wake2


	.set  min_tsens_meas2, 0 - 0
	.set  max_tsens_meas2, 4095 + 0
	.set  min_tsens_wait2, 0 - 0
	.set  max_tsens_wait2, 16383 + 0
	.global  min_tsens_meas2
	.global  max_tsens_meas2
	.global  min_tsens_wait2
	.global  max_tsens_wait2

	.set  min_adc_sel2, 0 - 0  
	.set  max_adc_sel2, 1 + 0  
	.set  min_adc_mux2, 0 - 0  
	.set  max_adc_mux2, 15 + 0  
	.set  min_adc_cyc2, 0 - 0
	.set  max_adc_cyc2, 65535 + 0

	.global  min_adc_sel2
	.global  max_adc_sel2
	.global  min_adc_mux2
	.global  max_adc_mux2
	.global  min_adc_cyc2
	.global  max_adc_cyc2

	.set  min_reg_Addr2, 0 - 0
	.set  max_reg_Addr2, 1023 + 0
	.set  min_reg_High2, 0 - 0
	.set  max_reg_High2, 31 + 0
	.set  min_reg_Low2, 0 - 0
	.set  max_reg_Low2, 31 + 0
	.set  min_reg_data2, 0 - 0
	.set  max_reg_data2, 255 + 0
	.global  min_reg_Addr2
	.global  max_reg_Addr2
	.global  min_reg_High2
	.global  max_reg_High2
	.global  min_reg_Low2
	.global  max_reg_Low2
	.global  min_reg_data2
	.global  max_reg_data2



   	.global  _temp_dataw
        _temp_dataw: .int    1111
   	.global  _temp_dataw331
        _temp_dataw331: .int    1234567
   	.global  _temp_dataw2
        _temp_dataw2: .int    1234567
