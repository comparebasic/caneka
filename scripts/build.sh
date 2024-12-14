#!/bin/sh
file=dist/src/secure/phrase.c
if [ -e $HOME/cb_phrase.txt -a ! -e $file ]; then
    mkdir -p dist/src/secure/
    echo  "#include <external.h>" > $file
    echo "#include <caneka.h>" >> $file
    echo "" >> $file
    echo "String *CB_Phrase(MemCtx *m){" >> $file
    echo -n "    return Cont(m, bytes(\"" >> $file
    cat "${HOME}/cb_phrase.txt" >> $file
    echo "\"));" >> $file
    echo "}" >> $file
fi
if [ -e ./build/caneka ]; then
    ./build/caneka transp
fi
if [ ! -e ./build/build ] ; then
    clang -o build/build dist/src/build.c
fi
./build/build
