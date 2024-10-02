#!/bin/sh
mkdir -p build
rm -Rf build/*
clang -o build/build src/build.c
./build/build
