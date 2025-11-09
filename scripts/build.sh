#!/bin/sh

CC="clang"
INC="-I ./src/include -I ./src/base/include -I ./src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

mkdir -p ./build/libcaneka/
echo "building Caneka Base" 
$CC -g -Wno-gnu-folding-constant $INC -c -o ./build/libcaneka/libcaneka.a ./src/base/inc.c -DINSECURE;

r=$?
if [ $r -ne 0 ]; then
    exit $?;
fi

mkdir -p ./build/libbuilder/
echo "building Caneka Builder"
$CC -g $INC -c -o ./build/libbuilder/libbuilder.a ./src/builder/inc.c -DINSECURE;

r=$?
if [ $r -ne 0 ]; then
    exit $r;
fi

exit


echo "building Caneka Build Config"
$CC -o build/build_ext $INC $STATICS src/ext/build.c -lm;

r=$?
if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Caneka Ext..."
./build/build_ext && $CC -o build/build_lang $INC $STATICS src/lang/build.c -lm;

r=$?
if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Caneka Lang..."
./build/build_lang && $CC -o build/build_www $INC $STATICS src/www/build.c -lm;

r=$?
if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Caneka Www..."
./build/build_www && $CC -o build/build_nacl $INC $STATICS src/third/nacl/build.c -lm;

r=$?
if [ $r -ne 0 ]; then
    exit $r;
fi

echo "building Caneka Nacl..."
./build/build_nacl

r=$?
if [ $r -ne 0 ]; then
    exit $r;
fi

echo "Core modules built successfully"
