#!/bin/sh
CC="clang"

mkdir -p ./build/libcnkbase/ && \
echo "building Caneka Base" && \
$CC -g -Wno-gnu-folding-constant -I artifact/src/include -c \
    -o ./build/libcnkbase/libcnkbase.a ./artifact/src/base/inc.c -DINSECURE && \
mkdir -p ./build/libbuilder/ && \
echo "building Caneka Builder" && \
$CC -g -I ./artifact/src/include -I ./artifact/src/programs/builder/include -c \
    -o ./build/libbuilder/libbuilder.a ./artifact/src/builder/builder.c -DINSECURE && \
echo "building Caneka Build Config" && \
$CC -o build/build_ext -I artifact/src/include build/libcnkbase/libcnkbase.a build/libbuilder/libbuilder.a artifact/src/ext/build.c -lm && \
echo "building Caneka Ext..." && \
./build/build_ext
$CC -o build/build_www -I artifact/src/include build/libcnkbase/libcnkbase.a build/libbuilder/libbuilder.a artifact/src/www/build.c -lm && \
echo "building Caneka Www..." && \
./build/build_www
