#!/bin/sh
clang -o build/builder builder.c && ./build/builder build && \
clang -o build/builder builder.c && ./build/builder artifact/src/build.c  && \
./build/build
