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

${AS} -al ./gas/testsuite/gas/esp32ulp/i2c_wr.s > ./gas/testsuite/gas/esp32ulp/compare/i2c_wr.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/esp32ulp_all.s > ./gas/testsuite/gas/esp32ulp/compare/esp32ulp_all.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/esp32ulp_globals.s > ./gas/testsuite/gas/esp32ulp/compare/esp32ulp_globals.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/esp32ulp_ranges.s > ./gas/testsuite/gas/esp32ulp/compare/esp32ulp_ranges.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/test_and.s > ./gas/testsuite/gas/esp32ulp/compare/test_and.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/test_jumpr.s > ./gas/testsuite/gas/esp32ulp/compare/test_jumpr.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/test_reg.s > ./gas/testsuite/gas/esp32ulp/compare/test_reg.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/test1.s > ./gas/testsuite/gas/esp32ulp/compare/test1.lst
${AS} -al ./gas/testsuite/gas/esp32ulp/test_macro.s > ./gas/testsuite/gas/esp32ulp/compare/test_macro.lst
