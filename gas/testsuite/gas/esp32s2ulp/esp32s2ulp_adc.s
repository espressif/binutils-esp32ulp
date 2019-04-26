        .data                                   //Data section definition
Addr1:  .word     123                           // Define label Addr1 16 bit
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
    .global __start

__start:
        
        .text                                   //Text section definition
    	
	ADC   R1, 0, 0, 0
	ADC   R1, min_adc_sel, min_adc_mux, min_adc_cyc
	ADC   R1, 1, 15, 0xffff
	ADC   R1, max_adc_sel, max_adc_mux, max_adc_cyc

	ADC   R1, min_adc_sel2, min_adc_mux2, min_adc_cyc2
	ADC   R1, max_adc_sel2, max_adc_mux2, max_adc_cyc2
