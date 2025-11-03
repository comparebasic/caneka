#!/bin/sh
CC="clang"
INC="-I ./artifact/src/include -I artifact/src/base/include -I artifact/src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

./scripts/make.sh && \
echo "building BinSegCli Config..." && \
$CC -o build/programs_binseg-cli_build $INC $STATICS artifact/src/programs/binseg-cli/build.c -lm && \
echo "building Tests..." && \
./build/programs_binseg-cli_build
