#!/bin/sh
CC="clang"

./scripts/make.sh && \
echo "building Tests Config..." && \
$CC -o build/programs_tests_build -I artifact/src/include build/libcnkbase/libcnkbase.a \
    build/libcnkwww/libcnkwww.a build/libbuilder/libbuilder.a artifact/src/programs/tests/build.c -lm && \
echo "building Tests..." && \
./build/programs_tests_build && ./build/tests
