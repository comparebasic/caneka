# Caneka

The Glass-Bottom-Boat of Programming Language Runtimes.


## Overview

What could you do if you knew what happened between your programming
language and the hardware?

Caneka is a full-stack + web-server + database replacement for a web
development runtime. Based entirely on simplistic components written
from scratch in C.


## Licence(s)

Most of the system is licenced under a *3-Clause BSD Licence* from
[Compare Basic](https://comparebasic.com) with a few folders shared 
as public domain.  see [LICENCE](./LICENCE) file in the root directory 
for details about the source code licence(s) in other directories.


## Purpose

Caneka was born from the idea that productivity in sofware engineering
was in need of new tooling. To us, the evolution seamed to stall after
the OOP languages.

- Level 1 Assembly
- Level 2 Types (C)
- Level 3 Objects (C++, Java, JavaScript, Python, Etc.)
- Level 4 Composition/Implicit Handling (?)

It seams like we never got to that Level 4 set of languages.
A place where developers spend more time on composition than
repetitive details.

Lisp, Scala, Rust, and C# all came close with pattern matching and
routing features, but there is more power that developers could wield
(mwuahahaha... :);


## Why Start From Scratch(ish... built on top of POSIX, C and LibC)

The determination to start from scratch is an effort to accomplish a
syntax-agnostic runtime while building transparency and reliability at
the same time.

This also solves the "many dependencies" problem, by encompasing every
essential feature of a software application in one place. Caneka does not 
require any features outside of a POSIX compliant operating system and
a C compiler.

Components are linked as necessary for features such as crypto and 
network handling (such as NaCL and eventually SSL). And the build system
is designed for extensibility with other software sources.

It also increases portability to keep the C compilation step in place
so that it can build and run on a variety of architectures.


## The Parser

Think of it like "a regular expression engine that can generate and run data
structures".

The goal of this approach is to increase configuration and encourage teams to
map their problem space more directly, using less lines of boiler-plate code.

This is commonly refered to as a Domain Specific Language.

Since the dawn of computing, all computer tasks have been accomplished through
a combination of user-interaction, protocols, markups, and programming
langauges. Caneka aims to allow software development to include the definition
of the langauge syntax. 

For this analogy, here are examples from each category:

- Protocols: DNS, HTTP, SMTP/LMTP
- Protocols (binary): TLS, WebSockets, TCP/IP, Bluetooth, WiFi, Serial
- Markups: CSV, XML, JSON, YAML, CBOR, HTML, LaTeX, PDF
- Programming Langauges: Java, Python, TypeScript/Javascript, Go, Scala, Rust...

The comonality between items within each category speaks to the value of
customizaton, and also highlights the challenges of conformity.

Parser sources can be found in the [parser](/src/ext/parser/) folder or any
file that ends in \_roebling.c which is the naming convention for a `Roebling`
object source file.


## Caneka Components

Caneka has three main components including a parser with no prefered syntax, a
network server, and a memory manager for data storage and data structures.

Components:

- [base](src/base/): includes virtual memory segmented strings,
  scalable arrays, a key-value lookup, and the memory manager.
- [ext](/src/ext/): All other features, including the Seel class system
  found in [types](/src/ext/types). This includes the
  [parser](/src/ext/parser/) and the network server [serve](/src/ext/serve/) 
  which relies heavily on the Task/Step components in the
  [navigate](/src/ext/navigate/) sub-module.
- [inter](/src/inter) Higher level components such as HTTP handling
  an HTTP Webserver including session handling and user management.
- [programs](/src/programs/): Any programs that ship with the source
  will be in this folder, presently [test](src/programs/test/), a webserver, 
  and basic cli are in this folder.

see [caneka.org](https://caneka.org) for more details.


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
folder). The file [bootstrap.c](./bootstrap.c) in the root directory builds a
which menu to manage the build process.

Each folder within the [src](/src) artifact folder, have a file in
them which controls how they are built. Either as one include file such as (in
the case of [base/inc.c](/src/base/inc.c) or with a more eleborate
"build.c" file. Every module after the "base" is built using the "build.c"
file, which is used by "buildeka" builder.

Further customization can be found in the dependencies.txt files for each
module. 


## Why is itt So Hard To Read this Source Code? 

In it's present form, Caneka is not intended to be modified by human authors.
What you see now is inteneded to be generated by a transpiler which has
not yet been written. You will notice that a lot of rudimentary calls such as
`Table_Set`, `MemCh_AllocOf` and `StrVec_AddVec` are increadibly verbose
and unfriendly to read. This is by design with the intention of a cleaner
syntax sitting on top of this.

Preserving the ability to debug and perform all necissary tasks simplisticly
has been the focus this far. The system will round the corder into readability
in the near future.

Some of the code sit's between C and Assembly and will probably end up
being written in Assembly. [Iter](src/base/mem/iter.c) and
[Span](src/base/mem/span.c) in particular has a lot of `void *` casting
to implement the offset math that could be cleaner (and will have to be
in order to run on desktop and mobile targets).


## Build Status

The core components run pretty well and have been lightly tested, you
can see the status of the system at [caneka.org](https://caneka.org)
or by building and running the tests through the `bootstrap` program
option (see Build Steps above).

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
