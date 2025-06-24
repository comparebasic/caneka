#!/bin/sh

clang -g -I ./artifact/src/include -I ./artifact/src/programs/fmt/include \
    -o ./build/fmt ./artifact/src/programs/fmt/main.c \
    ./build/libcnkbase/libcnkbase.a ./build/libcaneka/libcaneka.a -lm -DINSECURE
