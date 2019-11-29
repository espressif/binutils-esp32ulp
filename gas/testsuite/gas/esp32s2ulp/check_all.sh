#!/bin/bash

AS_PATH="./gas/testsuite/gas/esp32s2ulp"

${AS_PATH}/check_as.sh "$@"
${AS_PATH}/check_as_ld.sh "$@"
