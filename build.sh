#!/bin/sh
echo "= Caneka build script - see README.md for details"
if [ $(which clang) ]; then
    CC="clang"
    echo "clang found using that for compilation"
elif [ $(which gcc) ]; then
    CC="gcc"
    echo "gcc found using that for compilation"
fi    

if [ -e ./dist/bootstrap ]; then
    echo "dist/bootstrap found running clean"
    ./dist/bootstrap --clean
fi

cmd="mkdir -p ./dist/bin"
echo "making dir $cmd"
$cmd;

cmd="$CC -o ./dist/bin/bootstrap -I src/programs/buildeka/include/ src/programs/buildeka/bootstrap.c"
echo "building bootstrap: $cmd"
$cmd

cmd="./dist/bin/bootstrap --menu"
echo "running bootstrap: $cmd"
$cmd
