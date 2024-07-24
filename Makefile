CC = /usr/bin/clang 
CFLAGS = --target=x86_64-unknown-linux-musl -g -Werror -Wno-incompatible-pointer-types-discards-qualifiers
INC = -I/usr/local/musl/include/ -I./include 

coreobj = core/caneka.o core/mem.o core/string.o core/error.o core/debug.o \
	core/log.o core/slab.o core/span.o core/lookup.o core/chain.o core/testsuite.o \
    core/hash.o core/table.o core/compare.o core/iter.o core/array.o core/int.o \
    core/maker.o
parserobj = parser/parsers.o parser/match.o parser/roebling.o parser/scursor.o parser/range.o \
    parser/mark.o
serveobj = serve/req.o serve/serve.o serve/proto.o serve/proto/headers/parserdef.o \
    serve/proto/headers/utils.o
httpobj = serve/proto/http/protodef.o serve/proto/http/proto.o serve/proto/http/parserdef.o \
    serve/proto/http/pat_matches.o serve/proto/http/debug.o
xmlobj = formats/xml/parserdef.o
testobj = tests/core_tests.o tests/string_tests.o tests/serve_tests.o \
	tests/span_tests.o tests/hash_tests.o tests/roebling_tests.o tests/proto_http_tests.o \
    tests/table_tests.o

allobj = $(coreobj) $(testobj) $(parserobj) $(httpobj) $(serveobj) $(xmlobj)

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
	mkdir -p build/serve/proto/headers
	mkdir -p build/tests
	mkdir -p build/proto/http
	mkdir -p build/formats/xml

$(allobj): %.o:%.c 
	$(CC) -c $(CFLAGS) $(INC) $< -o build/$@
