# Caneka

Caneka is a language toolset written in C, for the main purpose of writing Domain Specific Langauges, and producing minimal yet scalable runtimes.

It is memory managed with it's own manually-run garbage collector, and focuses on Cycle-Oriented-Programming as the main paradigm.

see [caneka.org](https://caneka.org) for more details.

## Build Instructions

Canka requires a small build program to be compiled, which can then build the rest of the system by launching other processes.

    ./make.sh

Further configuration can be specified at the top of [build_config.h](./build_config.h)

## Key Source Files

Here is a list of the cornerstone source code files of the system, each has a comment at the top about it's purpose and related source code files:

- [include/caneka.h](./include/caneka.h) - main include

This defines the types and structs for the runtime-polymorphic type system, and
all of Caneka.

- [include/mem.h](./include/mem.h) - Memory Context 

This is the manually-run garbage collector.

- [include/span.h](./include/span.h) - Data Strucutre for slab allocation of segmented arrays.

This is the main work-horse data structure for storing and retrieving data, Table is biult on top of this.

- [include/string_cnk.h](./include/string_cnk.h) - Data Strucutre for slab allocation of segmented arrays.

This is the main work-horse data structure for storing and retrieving data, Table is biult on top of this.

- [include/debug.h](./include/debug.h) - Configuration file for showing debugging information, the debug flags are located in core/debug.c

## Licence

MIT: [licence file](./LICENCE)

Copyright 2023-2024 COMPARE BASIC INCORPORATED

Caneka is a brand of Compare Basic, read more at [comparebasic.com](https://comparebasic.com)
