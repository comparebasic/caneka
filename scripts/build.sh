#!/bin/sh

CC="clang"
INC="-I ./src/include -I ./src/base/include -I ./src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

mkdir -p ./build/libcaneka/
echo "building Caneka Base" 
$CC -g -Wno-gnu-folding-constant $INC \
    -c -o ./build/libcaneka/libcaneka.a ./src/base/inc.c -DINSECURE || exit 1;

mkdir -p ./build/libbuilder/
echo "building Caneka Builder"
$CC -g $INC -c -o ./build/libbuilder/libbuilder.a ./src/builder/inc.c -DINSECURE \
    || exit 1;

THIRD=""
if [ $2 -eq "--all" ] ; then
    THIRD="-DCRYPTO"
fi

echo "building Caneka Ext"
$CC -o build/build_ext $INC $STATICS $THIRD src/ext/build.c -lm \
    && ./build/build_ext  || exit 1;

echo "building Caneka Inter..."
$CC -o build/build_inter $INC $STATICS src/inter/build.c -lm \
    && ./build/build_inter || exit 1;

if [ $2 -eq "--all" ] ; then
    echo "building Caneka Nacl..."
    $CC -o build/build_nacl $INC $STATICS src/third/nacl/build.c -lm && \
        ./build/build_nacl || exit 1;
fi

echo "Core modules built successfully"
