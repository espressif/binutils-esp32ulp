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

${AS} -al ${AS_PATH}/esp32s2ulp_st.s > ${AS_PATH}/compare/esp32s2ulp_st.lst
${AS} -al ${AS_PATH}/esp32s2ulp_regrw.s > ${AS_PATH}/compare/esp32s2ulp_regrw.lst
${AS} -al ${AS_PATH}/esp32s2ulp_i2crw.s > ${AS_PATH}/compare/esp32s2ulp_i2crw.lst
${AS} -al ${AS_PATH}/esp32s2ulp_wait.s > ${AS_PATH}/compare/esp32s2ulp_wait.lst
${AS} -al ${AS_PATH}/esp32s2ulp_adc.s > ${AS_PATH}/compare/esp32s2ulp_adc.lst
${AS} -al ${AS_PATH}/esp32s2ulp_alu.s > ${AS_PATH}/compare/esp32s2ulp_alu.lst
${AS} -al ${AS_PATH}/esp32s2ulp_jump.s > ${AS_PATH}/compare/esp32s2ulp_jump.lst
${AS} -al ${AS_PATH}/esp32s2ulp_jumpr.s > ${AS_PATH}/compare/esp32s2ulp_jumpr.lst
${AS} -al ${AS_PATH}/esp32s2ulp_wakehalt.s > ${AS_PATH}/compare/esp32s2ulp_wakehalt.lst
${AS} -al ${AS_PATH}/esp32s2ulp_tsens.s > ${AS_PATH}/compare/esp32s2ulp_tsens.lst
${AS} -al ${AS_PATH}/esp32s2ulp_sleep.s > ${AS_PATH}/compare/esp32s2ulp_sleep.lst
${AS} -al ${AS_PATH}/esp32s2ulp_ld.s > ${AS_PATH}/compare/esp32s2ulp_ld.lst
