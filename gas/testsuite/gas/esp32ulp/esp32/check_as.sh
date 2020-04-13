#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"
AS_PATH="./gas/testsuite/gas/esp32ulp/esp32"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
fi

echo "Testing $AS ..."

${AS} -al ${AS_PATH}/i2c_wr.s > ${AS_PATH}/compare/i2c_wr.lst --mcpu=esp32
echo "i2c_wr done ..."
${AS} -al ${AS_PATH}/esp32ulp_all.s > ${AS_PATH}/compare/esp32ulp_all.lst --mcpu=esp32
echo "esp32ulp_all done ..."
${AS} -al ${AS_PATH}/esp32ulp_globals.s > ${AS_PATH}/compare/esp32ulp_globals.lst --mcpu=esp32
echo "esp32ulp_globals done ..."
${AS} -al ${AS_PATH}/esp32ulp_ranges.s > ${AS_PATH}/compare/esp32ulp_ranges.lst --mcpu=esp32
echo "esp32ulp_ranges done ..."
${AS} -al ${AS_PATH}/test_and.s > ${AS_PATH}/compare/test_and.lst --mcpu=esp32
echo "test_and done ..."
${AS} -al ${AS_PATH}/test_jumpr.s > ${AS_PATH}/compare/test_jumpr.lst --mcpu=esp32
echo "test_jumpr done ..."
${AS} -al ${AS_PATH}/test_reg.s > ${AS_PATH}/compare/test_reg.lst --mcpu=esp32
echo "test_reg done ..."
${AS} -al ${AS_PATH}/test1.s > ${AS_PATH}/compare/test1.lst --mcpu=esp32
echo "test1 done ..."
${AS} -al ${AS_PATH}/test_macro.s > ${AS_PATH}/compare/test_macro.lst --mcpu=esp32
echo "test_macro done ..."
${AS} -al ${AS_PATH}/esp32ulp_jumpr.s > ${AS_PATH}/compare/esp32ulp_jumpr.lst --mcpu=esp32
echo "esp32ulp_jumpr done ..."
