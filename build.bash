#!/bin/bash

# -e exit immediately when a command fails.
# -u treat unset variables as an error and exit immediately.
# -x print each command before executing it.
# -o pipefail sets the exit code of a pipeline to that of the rightmost command to exit with a non-zero status.
set -euxo pipefail

mkdir -p ./bin

# -Wno-unused-variable -Wno-unused-parameter
cc -std=c18 -O0 -Werror -Wall -Wpedantic -Wextra -Wenum-conversion -Wassign-enum $1 -o ./bin/$1.out && ./bin/$1.out
