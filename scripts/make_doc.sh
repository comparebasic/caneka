#!/bin/sh

clang -o build/builder builder.c && ./build/builder artifact/src/programs/doc/build.c && \
./build/programs_doc_build
