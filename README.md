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

- [base](artifact/src/base/): includes virtual memory segmented strings,
  scalable arrays, a key-value lookup, and the memory manager.
- [ext](/artifact/src/ext/): all other features are found in the extended
  sources in the folder ext. This includes the [parser](/artifact/src/ext/parser/)
  and a few exmaple formats in the [format](/artifact/src/ext/format/) folder.
- [programs](/artifact/src/programs/): Any programs that ship with the source
  will be in this folder, presently [cnkbuild](/artifact/src/programs/cnkbuild) and
  [tests](artifact/src/programs/tests) are the only things that are included in
  the caneka sources.
- [deprecated](/artifact/src/deprecated/): some features have not been moved over
  since the new base was written and can be found in the deprecated folder. All
  of these features will eventually move to the `ext` folder.

see [caneka.org](https://caneka.org) for more details.

## Build Instructions

Canka only requires a C compile to build. A small build program is then used to
build the rest of the runtime.

    ./scripts/make.sh

The ./scripts/build.sh script base compiles a small builder program which
builds the remaining objects.

    clang -o build/builder builder.c && ./build/builder

If `gcc` is used instead of `clang` it will build the remaining objects using
the compiler supplied to build the builder, in that case `gcc` and eventually
`msvc` if that's what was used to build the builder.

Further customization can be found in the [build.c](./artifact/src/build.c)
file.

## Build Status

The system is currently being updated to include a new base, all components
have worked in prototype form at one time or another, but it's currently a
shit-show.

## Folder Layout

Because Caneka is intended to be written in another syntax and transpiled into
C, most of the actively developed source code is currently locaed in the
[artifact src folder](./artifact/src/). This is because, over time, caneka will
be transpiled from it's own, more convienient syntax.

## Licence

BSD 3 Clause: [licence file](./LICENCE)

Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
