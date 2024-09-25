#!/bin/sh
mkdir -p build
rm -Rf build/*
clang -o build/build build.c
./build/build
