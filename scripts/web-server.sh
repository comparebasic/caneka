#!/bin/sh
CC="clang"
INC="-I ./src/include -I src/base/include -I src/builder/include"
STATICS="build/libcaneka/libcaneka.a build/libbuilder/libbuilder.a"

echo "building WebServer Config..." && \
$CC -o build/programs_web-server_build $INC $STATICS src/programs/web-server/build.c -lm && \
echo "building Tests..." && \
./build/programs_web-server_build
