#!/bin/sh
clang -o build/rblsh_build -I dist/src/include/  dist/src/programs/rblsh/build.c
./build/rblsh_build
