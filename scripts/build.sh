#!/bin/sh

if [ $(which clang) ]; then
    CC="clang"
elif [ $(which gcc) ]; then
    CC="gcc"
else
    echo "No compiler found look for clang and gcc, set CC in ./scripts/build.sh manually if you'd like";
    exit 1;
fi;

echo "Making directories...";
mkdir -p ./build/libcnkbase && \
mkdir -p ./build/bin && \

cmd="$CC -g -I ./src -c -o ./build/libcnkbase/libcnkbase.a ./src/base/inc.c"
echo "Building Caneka Base: $cmd";
eval $cmd

cmd="$CC -g -I ./src -o ./build/bin/cnkbuild ./src/programs/builder/inc.c ./build/libcnkbase/libcnkbase.a -lm"
echo "Building Caneka Builder $cmd";
eval $cmd

cmd="./build/bin/cnkbuild --src src/programs/test"
echo "Running Builder $cmd";
eval $cmd

exit 1;
