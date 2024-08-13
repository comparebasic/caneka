#!/bin/sh
rm -Rf build/*
mkdir -p build
clang -o build/build build.c
