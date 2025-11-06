#!/bin/sh
CC="clang"
INC="-I ./src/include -I src/base/include -I src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

./scripts/make.sh
r=$?

if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Tests Config..." && \
$CC -o build/programs_tests_build $INC $STATICS src/programs/tests/build.c -lm

if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Tests..." && ./build/programs_tests_build

if [ $r -ne 0 ]; then
    exit $r;
fi 

echo "running Tests..."
./build/bin/tests
