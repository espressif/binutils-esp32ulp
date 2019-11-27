#!/bin/bash

set -o errexit
set -o pipefail
set -o nounset

AS="./gas/as-new"
LD="./ld/ld-new"

AS_PATH="./gas/testsuite/gas/esp32ulp"

if [ -n "${1:-}" ]; then
	PREFIX="$1"
	AS="${PREFIX}as"
	LD="${PREFIX}ld"
fi

echo "Testing $AS together with $LD ..."

${AS} -al ${AS_PATH}/esp32ulp_jumpr.s -o ${AS_PATH}/esp32ulp_jumpr.elf
${LD} -o  ${AS_PATH}/esp32ulp_jumpr.bin -A elf32-esp32ulp ${AS_PATH}/esp32ulp_jumpr.elf
