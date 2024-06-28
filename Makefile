CC = /usr/bin/clang 
CFLAGS = --target=x86_64-unknown-linux-musl -g -Werror -Wno-incompatible-pointer-types-discards-qualifiers
INC = -I/usr/local/musl/include/ -I./include 

coreobj = core/caneka.o core/mem.o core/string.o core/error.o core/debug.o \
	core/log.o core/slab.o core/span.o core/lookup.o core/chain.o core/testsuite.o \
    core/hash.o core/table.o core/compare.o
parserobj = parser/parsers.o parser/match.o parser/roebling.o parser/scursor.o
serveobj = serve/req.o serve/serve.o serve/proto.o 
httpobj = serve/proto/http/proto.o serve/proto/http/parsers.o serve/proto/http/pat_matches.o serve/proto/http/debug.o
testobj = tests/core_tests.o tests/string_tests.o tests/serve_tests.o \
	tests/span_tests.o tests/hash_tests.o

allobj = $(coreobj) $(testobj) $(parserobj) $(httpobj) $(serveobj)

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
	mkdir -p build/serve/proto/http
	mkdir -p build/tests
	mkdir -p build/proto/http

$(allobj): %.o:%.c 
	$(CC) -c $(CFLAGS) $(INC) $< -o build/$@
