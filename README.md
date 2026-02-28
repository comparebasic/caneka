# Caneka

Spatial Oriented Programming Language runtime and tools.

Avoid reduction by utalizing context, positioning, and transparency.

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

### Programs included in the sources

- [Buildeka](./src/programs/buildeka/): This is the build program which builds the rest of the sources.
- [Clineka](./src/programs/clineka/): This is command-line-interface with a few quick tools to do useful things.
- [Test](./src/programs/test/): This is the test suite for testing various parts of the system.


### Components

- Core: Done ([Base](./src/mod/base)/[Ext](./src/mod/ext/))
- Utilities: Experimental ([Inter](./src/mod/inter/))
- Passport: Initial Development ([Passport](./src/mod/passport/))
- Syntax: Not Started
- Python Module: Build Only ([Pyneka](./src/lib/pyneka))
- OpenSsl Bindings: Experimental ([third/OpenSsl](./src/mod/third/openssl))

#### Core Components

The core system, including the [memory manager](./src/mod/base/mem/),
[parser](./src/mod/ext/parser/), [network server](./src/mod/ext/serve/),
[type](./src/mod/base/types/), and [higher-object](./src/mod/ext/types/) are complete yet
minimally tested.

#### Utility Components

Utility components such as the [templater](./src/mod/inter/templ/), and [website
components](./src/mod/inter/www/) are working but still experimental.

#### Syntax

The syntax for writing the Caneka langauge has not been started, and is not
anticipated to be completed until after several products are created using the
C Language calls directly.

This is partly because Compare Basic needs revenue, and partly because the
syntax will greatly benefit from greater experience using these tools in
real-world applications.

#### Python Module

There is a small wrapper to create a Caneka runtime inside of a Python 3
runtime. It only verifies that the internals are functioning for now.

The intended purpose is to use Caneka features such as the Roebling parser and
other modules that add value to an existing Python eco-system. Similar to
the way that SciPy extends python with a multi-dimensional array mechanism
using a different memory structure than python, Pyneka aims to provide a 
different matching and parsing function that natively found in Python.

To build this module select the menu option from `build.sh`. This will
build Pyneka and run the [install.py](./src/lib/pyneka/install.py) script.

example python cli session:

    >>> import pyneka
    >>> ctx = pyneka.PynekaCtx()
    >>> ctx.memstats()
    'MemStat<44k total/maxIdx=11/11 page-size=4096b>'

#### OpenSSL Bindings

The OpenSSL bindings are in place to do simpler crypto operations such as
evaluate signatures from key pairs and create cryptographic hashes. 

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
folder). The file [bootstrap.c](./src/programs/buildeka/bootstrap.c) builds a
command-line menu that can be used to manage the build process.

Further customization can be found in the dependencies.txt files for each
sub-folder. 


## License

The system is licenced under a *3-Clause BSD Licence*. See the
[LICENCE](./LICENSE) file in the root directory for details about the source
code licence(s) in other directories.


See [caneka.org](https://caneka.org) for more details.

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
