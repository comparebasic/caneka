#!/bin/sh
CC="clang"
INC="-I ./src/include -I src/base/include -I src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

if [ $2 -eq "--all" ] ; then
    TESTS="-DBASE -DEXT -DCRYPTO -DINTER" 
if [ $2 -eq "--non-third" ] ; then
    TESTS="-DBASE -DEXT -DINTER" 
fi

echo "building Tests Config..." && \
$CC -o build/programs_tests_build $INC $STATICS "$TESTS" src/programs/tests/build.c -lm

r=$?

if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Tests..." && ./build/programs_tests_build

r=$?

if [ $r -ne 0 ]; then
    exit $r;
fi 

echo "Tests build, run ./build/bin/tests to run them."
