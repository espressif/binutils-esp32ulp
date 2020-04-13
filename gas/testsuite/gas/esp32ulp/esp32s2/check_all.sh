#!/bin/bash

AS_PATH="./gas/testsuite/gas/esp32ulp/esp32s2"

${AS_PATH}/check_as.sh "$@"
${AS_PATH}/check_as_ld.sh "$@"
