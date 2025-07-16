#!/bin/sh

clang -g -I ./artifact/src/include -I ./artifact/src/programs/cnkbuild/include \
    -o ./build/build_doc ./artifact/src/programs/doc/build.c \
    ./build/libcnkbuild/libcnkbuild.a ./build/libcnkbase/libcnkbase.a -lm

./build/build_doc

clang -g -I ./artifact/src/include \
    -o ./build/doc \
    ./artifact/src/programs/doc/main.c \
    ./build/libcnkbase/libcnkbase.a ./build/libcaneka/libcaneka.a \
    ./build/libdoc/libdoc.a -lm
