CC = /usr/bin/clang 
CFLAGS = --target=x86_64-unknown-linux-musl -g -Werror
INC = -I/usr/local/musl/include/ -I./ 

obj = filestore.o scursor.o req.o serve.o mem.o string.o error.o debug.o \
	log.o tokens.o parsers.o array.o match.o \
     testsuite.o tests/string_tests.o tests/serve_tests.o

all: filestore

clean:
	rm -Rf build/*

libbasic.a: 
	cd /mnt/basic/basictypes && make

filestore: dirs $(obj)
	$(CC) $(CFLAGS) $(INC) $(LIBS) -o build/filestore main.c $(addprefix build/, $(obj))

dirs:
	mkdir -p build
	mkdir -p build/tests

$(obj): %.o:%.c 
	$(CC) -c $(CFLAGS) $(INC) $< -o build/$@
