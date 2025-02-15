#!/bin/sh
clang -o build/signer_build -I dist/src/include/  dist/src/programs/signer/build.c
./build/signer_build
