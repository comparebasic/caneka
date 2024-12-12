#!/bin/sh
mkdir -p build
rm -Rf build/*
rm -Rf dist/src/*
if [ -e $HOME/cb_phrase.txt ]; then
    mkdir -p src/secure/
    file=src/secure/phrase.c
    echo  "#include <external.h>" > $file
    echo "#include <caneka.h>" >> $file
    echo "" >> $file
    echo "String *CB_Phrase(MemCtx *m){" >> $file
    echo -n "    return Cont(m, bytes(\"" >> $file
    cat "${HOME}/cb_phrase.txt" >> $file
    echo "\"));" >> $file
    echo "}" >> $file
fi

clang -o build/build src/build.c
./build/build
