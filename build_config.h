#define CC "clang"
#define AR "ar"
#define CFLAGS cflags
#define INC inc 
#define BINARY "caneka"
#define MAIN "main.c"
#define ALL allobj
#define VERBOSE 2

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    NULL
};

static char *inc[] = {
    "-I./include",
    NULL
};

static BuildSubdir coreobj = { "core", {
    "caneka.c", "mem.c", "string.c", "error.c",
	"log.c", "spandef.c", "span.c", "slab.c", "lookup.c", "chain.c",
    "testsuite.c", "hash.c", "table.c", "compare.c", 
    "iter.c", "array.c", "int.c", "maker.c", "do.c", 
    "mess.c", "single.c", "debug.c", "abstract.c",
    NULL
}};

static BuildSubdir parserobj = {"parser", {
    "match.c", "roebling.c", "scursor.c", "range.c", NULL
}};

static BuildSubdir testobj= { "tests", {
    "span_setup_tests.c","core_tests.c", "string_tests.c", "table_tests.c",
    "match_tests.c", "span_tests.c", "hash_tests.c", "roebling_tests.c",
    /*
    "xml_tests.c",
    */
    NULL
}};

BuildSubdir *allobj[] = {&coreobj, &testobj, &parserobj, NULL};
