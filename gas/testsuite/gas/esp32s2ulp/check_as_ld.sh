#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"
LD="./ld/ld-new"

AS_PATH="./gas/testsuite/gas/esp32s2ulp"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
	LD="${PREFIX}ld"
fi

echo "Testing $AS together with $LD ..."

${AS} -al ${AS_PATH}/esp32s2ulp_regrw.s -o ${AS_PATH}/esp32s2ulp_regrw.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_regrw.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_regrw.elf

${AS} -al ${AS_PATH}/esp32s2ulp_jumpr.s -o ${AS_PATH}/esp32s2ulp_jumpr.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_jumpr.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_jumpr.elf

${AS} -al ${AS_PATH}/esp32s2ulp_st.s -o ${AS_PATH}/esp32s2ulp_st.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_st.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_st.elf

${AS} -al ${AS_PATH}/esp32s2ulp_i2crw.s -o ${AS_PATH}/esp32s2ulp_i2crw.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_i2crw.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_i2crw.elf

${AS} -al ${AS_PATH}/esp32s2ulp_wait.s -o ${AS_PATH}/esp32s2ulp_wait.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_wait.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_wait.elf

${AS} -al ${AS_PATH}/esp32s2ulp_adc.s -o ${AS_PATH}/esp32s2ulp_adc.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_adc.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_adc.elf

${AS} -al ${AS_PATH}/esp32s2ulp_alu.s -o ${AS_PATH}/esp32s2ulp_alu.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_alu.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_alu.elf

${AS} -al ${AS_PATH}/esp32s2ulp_jump.s -o ${AS_PATH}/esp32s2ulp_jump.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_jump.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_jump.elf

${AS} -al ${AS_PATH}/esp32s2ulp_wakehalt.s -o ${AS_PATH}/esp32s2ulp_wakehalt.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_wakehalt.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_wakehalt.elf

${AS} -al ${AS_PATH}/esp32s2ulp_tsens.s -o ${AS_PATH}/esp32s2ulp_tsens.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_tsens.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_tsens.elf

${AS} -al ${AS_PATH}/esp32s2ulp_sleep.s -o ${AS_PATH}/esp32s2ulp_sleep.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_sleep.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_sleep.elf

${AS} -al ${AS_PATH}/esp32s2ulp_ld.s -o ${AS_PATH}/esp32s2ulp_ld.elf --mcpu=esp32s2
${LD} -o ${AS_PATH}/compare/esp32s2ulp_ld.bin -A elf32-esp32ulp ${AS_PATH}/esp32s2ulp_ld.elf
