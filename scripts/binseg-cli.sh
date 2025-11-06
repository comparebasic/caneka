#!/bin/sh
CC="clang"
INC="-I ./src/include -I src/base/include -I src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

./scripts/make.sh && \
echo "building BinSegCli Config..." && \
$CC -o build/programs_binseg-cli_build $INC $STATICS src/programs/binseg-cli/build.c -lm && \
echo "building Tests..." && \
./build/programs_binseg-cli_build
