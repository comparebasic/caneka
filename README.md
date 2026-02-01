# Caneka

Spatial Oriented Programming Language. Runtime and tools. Avoids reduction by utalizing context,
positioning, and transparency.


## Overview

Spacial Oriented Programming provides programming languages that focus on 
two main things:

- What computers do for people.
- How solutions are communicated between engineers, or engineers and designers.

Caneka redefines computer programming as a pattern-definition excercise rather
than pattern-breakdown excercise. This removes the "argument call return"
convention found in all descendants of the C Programming langauge.

Unlike the appeal of functions and behavioral objects, which has captivated the
software industry for nearly 30 years, spatial oriented programming systems
rely on stacks, queues, and patterns to provide data-transformations. This
relies heavily on the assumption that computers are essentially
data-transformation systems.

The system is currently implemented in C and will likely remain that way.
However, the return-value feature of the C Langauge is rarely used. In most
cases, functions store all valuable information on an arguments which 
serves as a context.


## Status

The status of the system is available at [caneka.org](https://caneka.org)
or by building and running the [Test](./src/programs/test) program.

### Components

- Core: Done ([Base](./src/base)/[Ext](./src/ext/))
- Utilities: Experimental ([Inter](./src/inter/))
- Syntax: Not Started

### Programs included in the sources

- [Buildeka](./src/programs/buildeka/): This is the build program which builds the rest of the sources.
- [Clineka](./src/programs/clineka/): This is command-line-interface with a few quick tools to do useful things.
- [Test](./src/programs/test/): This is the test suite for testing various parts of the system.

#### Core Components

The core system, including the [memory manager](./src/base/mem/),
[parser](./src/ext/parser/), [network server](./src/ext/serve/),
[type](./src/base/types/), and [higher-object](./src/ext/types/) are complete yet
minimally tested.

#### Utility Components

Utility components such as the [templater](./src/inter/templ/), and [website
components](./src/inter/www/) are working but still experimental.

#### Syntax

The syntax for writing the Caneka langauge has not been started, and is not
anticipated to be completed until after several products are created using the
C Language calls directly.

This is partly because Compare Basic needs revenue, and partly because the
syntax will greatly benefit from greater experience using these tools in
real-world applications.


## Build Instructions

To build and test Caneka run using the helper script using *clang*:

    ./build.sh

and eventually, this will be support gcc and other compilers as well. Note:
there is a bug when compling with gcc related to the order of static libs.

This will compile and run the bootstrap program which will present a menu
to guide you through the rest of the build process.


### CnkBuild - source builder

Canka only requires a C compiler to build. A small library is built to manage
the build configuration(s) (found in the [buildeka](./src/programs/buildeka/)
folder). The file [bootstrap.c](./programs/buileka/bootstrap.c) builds a
command-line menu that can be used to manage the build process.

Further customization can be found in the dependencies.txt files for each
sub-folder. 


## Licence

The system is licenced under a *3-Clause BSD Licence*. See the
[LICENCE](./LICENCE) file in the root directory for details about the source
code licence(s) in other directories.


See [caneka.org](https://caneka.org) for more details.

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
