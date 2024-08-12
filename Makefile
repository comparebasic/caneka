all:
	clang -o build/build build.c && ./build/build

clean:
	rm -Rf build/*
