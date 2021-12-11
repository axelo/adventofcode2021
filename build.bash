#!/bin/bash

# -e exit immediately when a command fails.
# -u treat unset variables as an error and exit immediately.
# -x print each command before executing it.
# -o pipefail sets the exit code of a pipeline to that of the rightmost command to exit with a non-zero status.
set -euxo pipefail

if [ "${1-}" == "all" ]; then
    for i in {01..11}
    do
    ./$0 Day$(printf "%02d" $i).c ${2-}
    done
else
    mkdir -p ./bin

    # -Wpedantic Reject GNU C extensions
    WARNINGS="-Werror -Wall -Wpedantic -Wextra -Wenum-conversion -Wassign-enum -Wshadow"

    DISABLED_WARNINGS="-Wno-unused-variable -Wno-unused-parameter -Wno-unused-function"
    #DISABLED_WARNINGS=""

    OPTIMIZATION_LEVEL="-O0"

    DEFINES=""


    if [ "${2-}" == "example" ]; then
        DEFINES="-DEXAMPLE"
    fi

    # -fsanitize=address
    # -std=c18 Use C standard C18
    cc -std=c18 $OPTIMIZATION_LEVEL $WARNINGS $DISABLED_WARNINGS $DEFINES $1 -o ./bin/$1.out && ./bin/$1.out
fi
