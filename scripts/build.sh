#!/bin/bash
if [ -e ./build/caneka ]; then
    echo -e "\x1b[33m[Transpiling]\x1b[0m"
    ./build/caneka transp
fi
if [ ! -e ./build/libcnkbuild.a ] ; then
    echo -e "\x1b[33m[Building CnkBuild Library]\x1b[0m"
    mkdir -p build/libcnkbuild/
    clang -g -Wno-gnu-folding-constant -I artifact/src -I artifact/src/include -I artifact/src/programs/cnkbuild/include -c -o build/libcnkbuild/libcnkbuild.a artifact/src/programs/cnkbuild/cnkbuild.c
fi
if [ ! -e ./build/build ] ; then
    echo -e "\x1b[33m[Building Build Binary]\x1b[0m"
    clang -g -Wno-gnu-folding-constant -I artifact/src/include -o build/build artifact/src/build.c build/libcnkbuild/libcnkbuild.a
fi
# echo -e "\x1b[33m[Building...]\x1b[0m"
#./build/build
