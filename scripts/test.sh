#!/bin/sh
CC="clang"
INC="-I ./artifact/src/include -I artifact/src/base/include -I artifact/src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

./scripts/make.sh
r=$?

if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Tests Config..." && \
$CC -o build/programs_tests_build $INC $STATICS artifact/src/programs/tests/build.c -lm

if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Tests..." && ./build/programs_tests_build && ./build/bin/tests

if [ $r -ne 0 ]; then
    exit $r;
fi
