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

${AS} -al ${AS_PATH}/esp32s2ulp_jumpr.s -o ${AS_PATH}/esp32s2ulp_jumpr.elf
${LD} -o ${AS_PATH}/esp32s2ulp_jumpr.bin -A elf32-esp32s2ulp ${AS_PATH}/esp32s2ulp_jumpr.elf
