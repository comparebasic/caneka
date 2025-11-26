# Caneka

## The Anti-Syntax Runtime

Caneka was born from the idea that customized syntaxes can unlock the true
potential of humans using computers.

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

Protocols: DNS, HTTP, SMTP/LMTP
Protocols (binary): TLS, WebSockets, TCP/IP, Bluetooth, WiFi, Serial
Markups: CSV, XML, JSON, YAML, CBOR, HTML, LaTeX, PDF
Programming Langauges: Java, Python, TypeScript/Javascript, Go, Scala, Rust...

The comonality between items within each category speaks to the value of
customizaton, and also highlights the challenges of conformity.

# Caneka Components

Caneka has three main components including a parser with no prefered syntax, a
network server, and a memory manager for data storage and data structures.

Components:

- [base](src/base/): includes virtual memory segmented strings,
  scalable arrays, a key-value lookup, and the memory manager.
- [ext](/src/ext/): All other features, including the class system
  found in [types](/src/ext/types), are in the extended sources in the
  folder ext. This includes the [parser](/src/ext/parser/) and
  beginings of the [serve](/src/ext/serve/) folder. The server heavily
  leverages components in the [navigate](/src/ext/navigate/) module.
  To see how a session is componsed, view the
  [test](/src/programs/tests/tests/session_tests.c) file as a good
  reference.
- [programs](/src/programs/): Any programs that ship with the source
  will be in this folder, presently [tests](src/programs/tests) is the
  only meaningful program in this folder.

see [caneka.org](https://caneka.org) for more details.

## Build Instructions

To build and test Caneka run:

    ./scripts/make.sh && ./scripts/test.sh && ./build/bin/tests

There are other scripts in the [/scripts/](/scripts/) folder provide quick
commands.

Each folder within the [src](/src) artifact folder, have a file in
them which controls how they are built. Either as one include file such as (in
the case of [base/inc.c](/src/base/inc.c) or with a more eleborate
"build.c" file. Every module after the "base" is built using the "build.c"
file, which is used by "cnkbuild" builder.

### CnkBuild - source builder

Canka only requires a C compile to build. A small library is built to manage
the build configuration(s) (found in the [builder](./src/builder/)
folder). Small files named "build.c" are found throughout the codebase which 
build the objects and executables for the runtime.

Further customization can be found in the [build.c](./src/ext/build.c)
file, and the "build.c" files for each program, such as the 
[test program](./src/programs/tests/build.c).

## Build Status

The system is currently being updated to include a new base, all components
have worked in prototype form at one time or another, but it's currently a
shit-show.

## Folder Layout

There is a sketch of the CanekaLang syntax for the [Str](./src/base/str.cnk)
module, but it does not yet parser, it's what I've written as I begin to figure
out what the high-level syntax will look like.

Other examples of random things can be found in the [examples](/examples/) folder.

## Licence

BSD 3 Clause: [licence file](./LICENCE)

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
