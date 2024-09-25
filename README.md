# Caneka

Caneka is a language toolset written in C, for the main purpose of writing Domain Specific Langauges, and producing minimal yet scalable runtimes.

It is memory managed with it's own manually-run garbage collector, and focuses on Cycle-Oriented-Programming as the main paradigm.

see [caneka.org](https://caneka.org) for more details.

## Build instructions

    mkdir ./build
    clang -o ./build/build build.c
    ./build/build && ./build/caneka test

Copyright 2023-2024 COMPARE BASIC INCORPORATED
