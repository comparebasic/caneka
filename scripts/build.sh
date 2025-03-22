#!/bin/sh
if [ ! -e ./build/libcnkbase.a ] ; then
    echo "Building Base Library"
    mkdir -p build/libcnkbase/
    clang -g -Wno-gnu-folding-constant -I artifact/src/include -c -o build/libcnkbase/libcnkbase.a artifact/src/base/inc.c
    clang -g  -I artifact/src/include -I artifact/src/programs/basetests/include -o build/basetests \
        artifact/src/programs/basetests/tests.c artifact/src/programs/basetests/main.c
fi
#if [ ! -e ./build/build ] ; then
#    echo -e "\x1b[33m[Building Build Binary]\x1b[0m"
#    clang -g -Wno-gnu-folding-constant -I artifact/src/include -o build/build artifact/src/build.c build/libcnkbuild/libcnkbuild.a
#fi
#if [ -e ./build/caneka ]; then
#    echo -e "\x1b[33m[Transpiling]\x1b[0m"
#    ./build/caneka transp
#fi
# echo -e "\x1b[33m[Building...]\x1b[0m"
#./build/build
