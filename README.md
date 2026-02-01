# Caneka

Spatial Oriented Programming Language. Runtime and tools. Avoids reduction by utalizing context,
positioning, and transparency.


## Overview

Caneka aims to redefine programming as a pattern-definition excercise rather
than pattern-breakdown excercise. This replaces the "argument call return" model
of all descendants of the C Programming langauge, with pattern definitions instead.

Unlike the appeal of functions and behavioral objects, which has captivated the
software industry for nearly 30 years, spatial oriented programming systems
rely on stacks, queues, and patterns to provide data-transformations. This
relies heavily on the assumption that computers are essentially
data-transformation systems.

The system is currently implemented in C and will likely be that way for a long
time. The return-value feature of the C Langauge is rarely used. In most cases,
functions do not return any valuable information, this information is stored on
one of the arguments.


## What is Spacial Oriented Programming?

It is an effort to model a programming language as close as possible in two
ways. Firstly, what people need from computers. Secondly, how problems can be 
conceptualized among engineers, technology professionals, and designers.

Programming is done by specifying stacks, queues, and patterns, rather than
functions with arguments.

Programs tend to maintain contextual information througout a given task, rather
than consolidating and returning small pieces of informaton.

Status tends to be implicitly stored on a contextual object rather than returned
in an unrelated variable.

Behaviour is rarely associated with objects. Configuration is commonly used to
string together a pipeline that generates output from any given input.


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


### Programs included in the sources

[Buildeka](./src/programs/buildeka/): This is the build program which builds the rest of the sources.
[Clineka](./src/programs/clineka/): This is command-line-interface with a few quick tools to do useful things.
[Test](./src/programs/test/): This is the test suite for testing various parts of the system.


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


# A Note on Data-Science

There is no AI or Data-Science in Caneka at this time. Classical data-science
is planned using methodologies such as co-occurence, n-gram trees, or
map-reduce. Large-Langauge-Model methodologies have been completely avoided
because they rely on impersonation which makes them unreliable.


## Build Status

The core components run and have been lightly tested.
The status of the system is available at [caneka.org](https://caneka.org)
or by building and running the [Test](./src/programs/test) program.


## Licence

The system is licenced under a *3-Clause BSD Licence* from [Compare
Basic](https://comparebasic.com). See [LICENCE](./LICENCE) file in the root
directory for details about the source code licence(s) in other directories.


Caneka is a brand of Compare Basic, read more at
[comparebasic.com](https://comparebasic.com)
