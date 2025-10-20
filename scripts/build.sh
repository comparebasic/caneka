#!/bin/sh
CC="clang"
INC="-I ./artifact/src/include -I artifact/src/base/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

mkdir -p ./build/libcaneka/ && \
echo "building Caneka Base" && \
$CC -g -Wno-gnu-folding-constant $INC -c -o ./build/libcaneka/libcaneka.a ./artifact/src/base/inc.c -DINSECURE;

mkdir -p ./build/libbuilder/ && \
echo "building Caneka Builder" && \
$CC -g $INC -I ./artifact/src/programs/builder/include -c -o ./build/libbuilder/libbuilder.a ./artifact/src/builder/builder.c -DINSECURE;

echo "building Caneka Build Config" && \
$CC -o build/build_ext $INC $STATICS artifact/src/ext/build.c -lm;

echo "building Caneka Ext..." && \
./build/build_ext && \
$CC -o build/build_lang $INC $STATICS artifact/src/lang/build.c -lm;

echo "building Caneka Lang..." && \
./build/build_lang && \
$CC -o build/build_www $INC $STATICS artifact/src/www/build.c -lm;

echo "building Caneka Www..." && \
./build/build_www && \
$CC -o build/build_nacl $INC $STATICS artifact/src/third/nacl/build.c -lm;

echo "building Caneka Nacl...";
./build/build_nacl
