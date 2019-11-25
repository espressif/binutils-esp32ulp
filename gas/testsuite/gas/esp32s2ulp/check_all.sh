#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
fi

echo "Testing $AS ..."

${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_st.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_st.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_regrw.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_regrw.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_i2crw.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_i2crw.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_wait.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_wait.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_adc.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_adc.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_alu.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_alu.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jump.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_jump.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_wakehalt.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_wakehalt.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_tsens.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_tsens.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_sleep.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_sleep.lst
${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_ld.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_ld.lst
# This part used to check gas and linker together
#./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.s -o ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.elf
#./ld/ld-new.exe -o ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_jumpr.bin -A elf32-esp32s2ulp ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.elf
#
#${AS} -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.s > ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_jumpr.lst
#./gas/as-new -al ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.s -o ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.elf
#./ld/ld-new.exe -o ./gas/testsuite/gas/esp32s2ulp/compare/esp32s2ulp_jumpr.bin -A elf32-esp32s2ulp ./gas/testsuite/gas/esp32s2ulp/esp32s2ulp_jumpr.elf
