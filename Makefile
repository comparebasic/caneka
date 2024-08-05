UNAME := $(shell ) 
NETBSD := NetBSD
CC = 
CFLAGS =
INC =

coreobj = core/caneka.c core/mem.c core/string.c core/error.c core/debug.c \
	core/log.c core/slab.c core/span.c core/lookup.c core/chain.c core/testsuite.c \
    core/hash.c core/table.c core/compare.c core/iter.c core/array.c core/int.c \
    core/maker.c core/mess.c core/single.c
parserobj = parser/parsers.c parser/match.c parser/roebling.c parser/scursor.c parser/range.c
serveobj = serve/req.c serve/serve.c serve/proto.c serve/proto/headers/parserdef.c \
    serve/proto/headers/utils.c
httpobj = serve/proto/http/protodef.c serve/proto/http/proto.c serve/proto/http/parserdef.c \
    serve/proto/http/pat_matches.c serve/proto/http/debug.c
xmlobj = formats/xml/xml.c formats/xml/parserdef.c
testobj = tests/core_tests.c tests/string_tests.c tests/serve_tests.c \
	tests/span_tests.c tests/hash_tests.c tests/roebling_tests.c tests/proto_http_tests.c \
    tests/table_tests.c tests/xml_tests.c tests/match_tests.c

allobj = $(coreobj) $(testobj) $(parserobj) $(httpobj) $(serveobj) $(xmlobj)

all: setup caneka

setup:
	ifeq ($(UNAME), $(NETBSD))
	CC := cc
	CFLAGS := -g -Werror -Wno-incompatible-pointer-types-discards-qualifiers
	INC := -I./include 
	else
	CC := /usr/bin/clang 
	CFLAGS := --target=x86_64-unknown-linux-musl -g -Werror -Wno-incompatible-pointer-types-discards-qualifiers
	INC := -I/usr/local/musl/include/ -I./include 
	endif

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

$(allobj):
	@echo "Compiling $(allobj)"
	@echo $$UNAME
	for file in $(allobj); do \
		$(CC) -c $(CFLAGS) $(INC) $$name -o build/$$name; \
	done
