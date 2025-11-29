#!/bin/sh

#
# Run this command to view a quick set of choices for building/running/testing Caneka
#

echo -e "\n\033[33m  Hello, and welcome to the Caneka Build Helper\033[0m\n";
echo -e "  What would you like to do?\n    [1] Build and RUN Caneka and it's tests\n     2  Build Caneka only\n     3  Clean the ./build directory"
echo -e "     4  Build the CnkCli tools\n     5  Build the example WebServer\n     6  Build and RUN the example WebServer\n     7  Read some Documentation for Caneka"
echo -e "     8  Oops, Wrong Command\n\n";
echo -e "  enter a number or press Enter (default is 1)\n";

read choice 

runCmd=""
if [ "$choice" == "8" ]; then
    echo -e "\n\033[32m  Ok, See you next time!\033[0m\n"
    exit 1
elif [ "$choice" == "3" ]; then
    ./scripts/clean.sh
    exit $?
elif [ -z "$choice" ] || [ "$choice" == "1" ]; then
    runCmd="./build/bin/cnkbuild --src src/programs/test"
elif [ "$choice" == "4" ] || [ "$choice" == "5" ] || [ "$choice" == "6" ] || [ "$choice" == 7 ]; then
    echo -e "\n\033[33m  It's not quire ready but check https://caneka.org for details and release dates.\033[0m\n\n"
    exit 1;
fi

if [ $(which clang) ]; then
    CC="clang"
elif [ $(which gcc) ]; then
    CC="gcc"
else
    echo "No compiler found look for clang and gcc, set CC in ./scripts/build.sh manually if you'd like"
    exit 1
fi

echo "Making directories..."
mkdir -p ./build/libcnkbase && \
mkdir -p ./build/bin && \

cmd="$CC -g -I ./src -c -o ./build/libcnkbase/libcnkbase.a ./src/base/inc.c"
echo "Building Caneka Base: $cmd"
eval $cmd

cmd="$CC -g -I ./src -o ./build/bin/cnkbuild ./src/programs/builder/inc.c ./build/libcnkbase/libcnkbase.a -lm"
echo "Building Caneka Builder $cmd"
eval $cmd

if [ ! -z "$runCmd" ]; then
    cmd="./build/bin/cnkbuild --quiet --src src/programs/test"
    echo "Running Builder $cmd";
    eval $cmd
fi
