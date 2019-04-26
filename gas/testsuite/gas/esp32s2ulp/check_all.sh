./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_st.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_st.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_regrw.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_regrw.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_i2crw.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_i2crw.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_wait.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_wait.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_adc.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_adc.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_alu.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_alu.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jump.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_jump.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_wakehalt.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_wakehalt.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_tsens.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_tsens.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_sleep.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_sleep.lst
./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_ld.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_ld.lst
# This part used to check gas and linker together
#./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_st.s -o ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_st.elf
#./ld/ld-new.exe -o ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_st.bin -A elf32-esp32s2ulp ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_st.elf