#!/bin/sh
CC="clang"
INC="-I ./artifact/src/include -I artifact/src/base/include"
STATICS="build/libcnkbase/libcnkbase.a build/libbuilder/libbuilder.a"

mkdir -p ./build/libcnkbase/ && \
echo "building Caneka Base" && \
$CC -g -Wno-gnu-folding-constant $INC -c -o ./build/libcnkbase/libcnkbase.a ./artifact/src/base/inc.c -DINSECURE && \
mkdir -p ./build/libbuilder/ && \
echo "building Caneka Builder" && \
$CC -g $INC -I ./artifact/src/programs/builder/include -c -o ./build/libbuilder/libbuilder.a ./artifact/src/builder/builder.c -DINSECURE && \
echo "building Caneka Build Config" && \
$CC -o build/build_ext $INC $STATICS artifact/src/ext/build.c -lm && \
echo "building Caneka Ext..." && \
./build/build_ext && \
$CC -o build/build_www $INC $STATICS artifact/src/www/build.c -lm && \
echo "building Caneka Www..." && \
./build/build_www
