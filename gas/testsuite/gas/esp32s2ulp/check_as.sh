#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"

AS_PATH="./gas/testsuite/gas/esp32s2ulp"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
fi

echo "Testing $AS ..."

${AS} -al ${AS_PATH}/esp32s2ulp_st.s > ${AS_PATH}/compare/esp32s2ulp_st.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_regrw.s > ${AS_PATH}/compare/esp32s2ulp_regrw.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_i2crw.s > ${AS_PATH}/compare/esp32s2ulp_i2crw.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_wait.s > ${AS_PATH}/compare/esp32s2ulp_wait.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_adc.s > ${AS_PATH}/compare/esp32s2ulp_adc.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_alu.s > ${AS_PATH}/compare/esp32s2ulp_alu.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_jump.s > ${AS_PATH}/compare/esp32s2ulp_jump.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_jumpr.s > ${AS_PATH}/compare/esp32s2ulp_jumpr.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_wakehalt.s > ${AS_PATH}/compare/esp32s2ulp_wakehalt.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_tsens.s > ${AS_PATH}/compare/esp32s2ulp_tsens.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_sleep.s > ${AS_PATH}/compare/esp32s2ulp_sleep.lst --mcpu=esp32s2
${AS} -al ${AS_PATH}/esp32s2ulp_ld.s > ${AS_PATH}/compare/esp32s2ulp_ld.lst --mcpu=esp32s2
