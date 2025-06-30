#!/bin/sh

clang -g -I ./artifact/src/include -I ./artifact/src/programs/doc/include \
    -o ./build/doc ./artifact/src/programs/doc/main.c \
    ./build/libcnkbase/libcnkbase.a ./build/libcaneka/libcaneka.a -lm -DINSECURE
