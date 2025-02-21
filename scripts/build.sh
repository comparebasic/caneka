#!/bin/sh
./scripts/phrase.sh
if [ -e ./build/caneka ]; then
    ./build/caneka transp
fi
if [ ! -e ./build/libcnkbuild.a ] ; then
    clang -I dist/src -I dist/src/include -c -o build/libcnkbuild.a dist/src/builder/cnkbuild.c
    clang -I dist/src/include -o build/build dist/src/build.c build/libcnkbuild.a
fi
#if [ ! -e ./build/build ] ; then
#    clang -I dist/src/include -o build/build dist/src/build.c
#fi
#./build/build
