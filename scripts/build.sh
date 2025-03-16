#!/bin/sh
if [ -e ./build/caneka ]; then
    ./build/caneka transp
fi
if [ ! -e ./build/libcnkbuild.a ] ; then
    mkdir -p build/libcnkbuild/
    clang -I artifact/src -I artifact/src/include -I artifact/src/programs/cnkbuild/include -c -o build/libcnkbuild/libcnkbuild.a artifact/src/programs/cnkbuild/cnkbuild.c
fi
#if [ ! -e ./build/build ] ; then
#    clang -I artifact/src/include -o build/build artifact/src/build.c build/libcnkbuild/libcnkbuild.a
#fi
#./build/build
