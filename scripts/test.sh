#!/bin/sh
CC="clang"
INC="-I ./artifact/src/include -I artifact/src/base/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

./scripts/make.sh && \
echo "building Tests Config..." && \
$CC -o build/programs_tests_build $INC $STATICS artifact/src/programs/tests/build.c -lm && \
echo "building Tests..." && \
./build/programs_tests_build && ./build/tests
