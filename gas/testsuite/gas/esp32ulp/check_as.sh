#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"
AS_PATH="./gas/testsuite/gas/esp32ulp"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
fi

echo "Testing $AS ..."

${AS} -al ${AS_PATH}/i2c_wr.s > ${AS_PATH}/compare/i2c_wr.lst
${AS} -al ${AS_PATH}/esp32ulp_all.s > ${AS_PATH}/compare/esp32ulp_all.lst
${AS} -al ${AS_PATH}/esp32ulp_globals.s > ${AS_PATH}/compare/esp32ulp_globals.lst
${AS} -al ${AS_PATH}/esp32ulp_ranges.s > ${AS_PATH}/compare/esp32ulp_ranges.lst
${AS} -al ${AS_PATH}/test_and.s > ${AS_PATH}/compare/test_and.lst
${AS} -al ${AS_PATH}/test_jumpr.s > ${AS_PATH}/compare/test_jumpr.lst
${AS} -al ${AS_PATH}/test_reg.s > ${AS_PATH}/compare/test_reg.lst
${AS} -al ${AS_PATH}/test1.s > ${AS_PATH}/compare/test1.lst
${AS} -al ${AS_PATH}/test_macro.s > ${AS_PATH}/compare/test_macro.lst
${AS} -al ${AS_PATH}/esp32ulp_jumpr.s > ${AS_PATH}/compare/esp32ulp_jumpr.lst
