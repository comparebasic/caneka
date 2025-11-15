#!/bin/sh
CC="clang"
INC="-I ./src/include -I src/base/include -I src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

echo "building BinSegCli Config..." && \
$CC -o build/programs_cnkcli_build $INC $STATICS src/programs/cnkcli/build.c -lm && \
echo "building Tests..." && \
./build/programs_cnkcli_build
