#!/bin/sh
if [ -e ./build/caneka ]; then
    ./build/caneka transp
fi
if [ ! -e ./build/libcnkbuild.a ] ; then
    mkdir -p build/libcnkbuild/
    clang -I dist/src -I dist/src/include -c -o build/libcnkbuild/libcnkbuild.a dist/src/builder/cnkbuild.c
fi
if [ ! -e ./build/build ] ; then
    clang -I dist/src/include -o build/build dist/src/build.c build/libcnkbuild/libcnkbuild.a
fi
./build/build
