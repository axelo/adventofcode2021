#!/bin/bash

# -e exit immediately when a command fails.
# -u treat unset variables as an error and exit immediately.
# -x print each command before executing it.
# -o pipefail sets the exit code of a pipeline to that of the rightmost command to exit with a non-zero status.
set -euxo pipefail

mkdir -p ./bin

WARNINGS="-Werror -Wall -Wpedantic -Wextra -Wenum-conversion -Wassign-enum -Wshadow"

DISABLED_WARNINGS="-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function"
DISABLED_WARNINGS=""

OPTIMIZATION_LEVEL="-O0"

# -pedantic Reject GNU C extensions
# -std=c18 Use C standard C18

cc -std=c18 -pedantic $OPTIMIZATION_LEVEL $WARNINGS $DISABLED_WARNINGS $1 -o ./bin/$1.out && ./bin/$1.out
