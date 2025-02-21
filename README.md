# Caneka

Caneka is a language toolset written in C, for the main purpose of writing
Domain Specific Langauges, and producing minimal yet scalable runtimes.

It is memory managed with it's own manually-run garbage collector, and focuses
on Cycle-Oriented-Programming as the main paradigm.

see [caneka.org](https://caneka.org) for more details.

## Build Instructions

Canka requires a small build program to be compiled, which can then build the
rest of the system by launching other processes.

    ./scripts/make.sh

Further configuration can be specified at the top of
[build.c](./artifact/src/build.c)

To rebuild the code, the build command can be run directly:

   ./build/build

However, it does not detect changes to header files yet, so it is necessary to
run the full `make.sh` script to rebuild everything if header files are
changed.

## Build Status

Currently the system builds and runs on FreeBSD and Linux, and is expected to
build on any POSIX compatible operating system, including Mac, Windows, iOS,
and Android, though these platforms are untested.

## Tests

Tests can be found in the `tests` folder and are runnable from the `caneka`
binary with the argument `test`.

## Run Tests

Tests can be run using the following command

    ./build/caneka test

## Run Network Server

For example, on port 8000

    ./build/caneka serve=port:8000 

## Folder Layout

Because Caneka is intended to be written in another syntax and transpiled into
C, most of the actively developed source code is currently locaed in the
[artifact src folder](./artifact/src/). This is because, over time, caneka will
be transpiled from it's own, more convienient syntax.

## Licence

BSD 3 Clause: [licence file](./LICENCE)

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
