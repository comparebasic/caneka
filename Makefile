CC = /usr/bin/clang 
CFLAGS = --target=x86_64-unknown-linux-musl -g -Werror
INC = -I/usr/local/musl/include/ -I./include -I./http -I./

coreobj = core/filestore.o core/scursor.o core/req.o core/serve.o core/mem.o core/string.o \
    core/error.o core/debug.o core/log.o core/tokens.o core/parsers.o core/array.o core/match.o \
    core/structexp.o
httpobj = http/parsers.o http/pat_matches.o
testobj = core/testsuite.o tests/string_tests.o tests/serve_tests.o

allobj = $(coreobj) $(httpobj) $(testobj)

all: filestore

clean:
	rm -Rf build/*

libbasic.a: 
	cd /mnt/basic/basictypes && make

filestore: dirs $(allobj)
	$(CC) $(CFLAGS) $(INC) $(LIBS) -o build/filestore main.c $(addprefix build/, $(allobj))

dirs:
	mkdir -p build
	mkdir -p build/core
	mkdir -p build/tests
	mkdir -p build/http

$(allobj): %.o:%.c 
	$(CC) -c $(CFLAGS) $(INC) $< -o build/$@
