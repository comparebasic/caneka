#define CC "clang"
#define CFLAGS "-g -Werror -Wno-incompatible-pointer-types-discards-qualifiers"
#define INC "-I./include"
#define BINARY "caneka"
#define MAIN "main.c"
#define ALL allobj

static BuildSubdir coreobj = { "core", {
    "caneka.c", "mem.c", "string.c", "error.c",
	"log.c", "slab.c", "span.c", "lookup.c", "chain.c",
    "testsuite.c", "hash.c", "table.c", "compare.c", 
    "iter.c", "array.c", "int.c", "maker.c", "do.c", 
    "mess.c", "single.c", "debug.c", NULL
}};

static BuildSubdir parserobj = {"parser", {
    "match.c", "roebling.c", "scursor.c", "range.c", NULL
}};

static BuildSubdir testobj= { "tests", {
    "core_tests.c", "string_tests.c", "table_tests.c",  "xml_tests.c",
    "match_tests.c", "span_tests.c", "hash_tests.c",  "roebling_tests.c",
    NULL
}};

BuildSubdir *allobj[] = {&coreobj, &parserobj, &testobj, NULL};
