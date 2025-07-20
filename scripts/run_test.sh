#!/bin/sh
clang -o build/builder builder.c && ./build/builder artifact/src/programs/tests/build.c && 
./build/programs_tests_build && \
./build/tests
