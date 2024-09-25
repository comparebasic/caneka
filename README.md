# Caneka

Caneka is a language toolset written in C, for the main purpose of writing Domain Specific Langauges, and producing minimal yet scalable runtimes.

It is memory managed with it's own manually-run garbage collector, and focuses on Cycle-Oriented-Programming as the main paradigm.

see [caneka.org](https://caneka.org) for more details.

## Build instructions

Canka requires a small build program to be compiled, which can then build the rest of the system by launching other processes.

    ./make.sh

Further configuration can be specified at the top of [build_config.h](./build_config.h)

## Licence

MIT: [licence file](./LICENCE)

Copyright 2023-2024 COMPARE BASIC INCORPORATED

Caneka is a brand of Compare Basic, read more at [comparebasic.com](https://comparebasic.com)
