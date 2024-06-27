CC = /usr/bin/clang 
CFLAGS = --target=x86_64-unknown-linux-musl -g -Werror
INC = -I/usr/local/musl/include/ -I./include 

coreobj = core/caneka.o core/mem.o core/string.o \
    core/error.o core/debug.o core/log.o core/slab.o core/span.o core/lookup.o core/chain.o
parserobj = core/parsers.o core/match.o core/roebling.o
serveobj = core/scursor.o core/req.o core/serve.o core/proto.o 
httpobj = proto/http/proto.o proto/http/parsers.o proto/http/pat_matches.o
testobj = core/testsuite.o tests/core_tests.o tests/string_tests.o tests/serve_tests.o \
    tests/span_tests.o

allobj = $(coreobj)  $(testobj) $(parserobj) $(servobj) $(httpobj)

all: caneka

clean:
	rm -Rf build/*

libbasic.a: 
	cd /mnt/basic/basictypes && make

caneka: dirs $(allobj)
	$(CC) $(CFLAGS) $(INC) $(LIBS) -o build/caneka main.c $(addprefix build/, $(allobj))

dirs:
	mkdir -p build
	mkdir -p build/core
	mkdir -p build/parser
	mkdir -p build/serve
	mkdir -p build/tests
	mkdir -p build/proto/http

$(allobj): %.o:%.c 
	$(CC) -c $(CFLAGS) $(INC) $< -o build/$@
