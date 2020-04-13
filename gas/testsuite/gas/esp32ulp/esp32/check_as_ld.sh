#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"
LD="./ld/ld-new"

AS_PATH="./gas/testsuite/gas/esp32ulp/esp32"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
	LD="${PREFIX}ld"
fi

echo "Testing $AS together with $LD ..."

${AS} -al ${AS_PATH}/i2c_wr.s -o ${AS_PATH}/i2c_wr.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/i2c_wr.bin -A elf32-esp32ulp ${AS_PATH}/i2c_wr.elf

${AS} -al ${AS_PATH}/esp32ulp_all.s -o ${AS_PATH}/esp32ulp_all.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/esp32ulp_all.bin -A elf32-esp32ulp ${AS_PATH}/esp32ulp_all.elf

${AS} -al ${AS_PATH}/esp32ulp_globals.s -o ${AS_PATH}/esp32ulp_globals.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/esp32ulp_globals.bin -A elf32-esp32ulp ${AS_PATH}/esp32ulp_globals.elf

${AS} -al ${AS_PATH}/esp32ulp_ranges.s -o ${AS_PATH}/esp32ulp_ranges.elf --mcpu=esp32
${AS} -al ${AS_PATH}/esp32ulp_globals.s -o ${AS_PATH}/esp32ulp_globals.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/esp32ulp_ranges.bin -A elf32-esp32ulp ${AS_PATH}/esp32ulp_ranges.elf ${AS_PATH}/esp32ulp_globals.elf

${AS} -al ${AS_PATH}/test_and.s -o ${AS_PATH}/test_and.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/test_and.bin -A elf32-esp32ulp ${AS_PATH}/test_and.elf

${AS} -al ${AS_PATH}/test_jumpr.s -o ${AS_PATH}/test_jumpr.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/test_jumpr.bin -A elf32-esp32ulp ${AS_PATH}/test_jumpr.elf

${AS} -al ${AS_PATH}/test_reg.s -o ${AS_PATH}/test_reg.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/test_reg.bin -A elf32-esp32ulp ${AS_PATH}/test_reg.elf

${AS} -al ${AS_PATH}/test1.s -o ${AS_PATH}/test1.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/test1.bin -A elf32-esp32ulp ${AS_PATH}/test1.elf

${AS} -al ${AS_PATH}/test_macro.s -o ${AS_PATH}/test_macro.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/test_macro.bin -A elf32-esp32ulp ${AS_PATH}/test_macro.elf

${AS} -al ${AS_PATH}/esp32ulp_jumpr.s -o ${AS_PATH}/esp32ulp_jumpr.elf --mcpu=esp32
${LD} -o  ${AS_PATH}/compare/esp32ulp_jumpr.bin -A elf32-esp32ulp ${AS_PATH}/esp32ulp_jumpr.elf
