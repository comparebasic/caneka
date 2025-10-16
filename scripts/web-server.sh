#!/bin/sh
CC="clang"
INC="-I ./artifact/src/include -I artifact/src/base/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

./scripts/make.sh && \
echo "building WebServer Config..." && \
$CC -o build/programs_web-server_build $INC $STATICS artifact/src/programs/web-server/build.c -lm && \
echo "building Tests..." && \
./build/programs_web-server_build
