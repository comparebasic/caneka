# Caneka

The Glass-Bottom-Boat of Programming Language Runtimes 

## Overview

What could you do if you know what happened between your programming
language and the hardware?

Caneka is a full-stack + web-server + database replacement for a web
development runtime. Based entirely on simplistic components written
from scratch in C.

## Purpose

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
  [inter](/src/inter) Higher level components such as HTTP handling
  an HTTP Webserver including session handling and user management.
- [programs](/src/programs/): Any programs that ship with the source
  will be in this folder, presently [tests](src/programs/tests) is the
  only meaningful program in this folder.

see [caneka.org](https://caneka.org) for more details.

## Build Instructions

To build and test Caneka run:

    clang -o bootstrap bootstrap.c && ./bootstrap 

and eventually, this will be supported:

    gcc -o bootstrap bootstrap.c && ./bootstrap 

Note there is a bug when compling with gcc related to the order of static libs.

This will compile and run the bootstrap program which manages building the rest
of the modules.

### CnkBuild - source builder

Canka only requires a C compiler to build. A small library is built to manage
the build configuration(s) (found in the [cnkbuild](./src/programs/cnkbuild/)
folder). The file [bootstrap.c](./bootstrap.c) in the root directory builds a
which menu to manage the build process.

Each folder within the [src](/src) artifact folder, have a file in
them which controls how they are built. Either as one include file such as (in
the case of [base/inc.c](/src/base/inc.c) or with a more eleborate
"build.c" file. Every module after the "base" is built using the "build.c"
file, which is used by "cnkbuild" builder.

Further customization can be found in the dependencies.txt files for each
module. 

## Build Status

The system is currently being updated to include a new base, all components
have worked in prototype form at one time or another, but it's currently a
shit-show.

## Licence

BSD 3 Clause: [licence file](./LICENCE)

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
