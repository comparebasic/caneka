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
    "-DFREEBSD",
    NULL
};

static char *inc[] = {
    "-I./src/include/", "-I./src/apps/include/",
    NULL
};

static BuildSubdir coreobj = { "core", {
    "caneka.c", "mem.c", "string.c", "error.c",
	"log.c", "spandef.c", "span.c", "spanquery.c", "slab.c", "lookup.c", "chain.c",
    "testsuite.c", "hash.c", "table.c", "compare.c", 
    "iter.c", "array.c", "int.c", "maker.c", "do.c", 
    "mess.c", "single.c", "debug.c", "abstract.c", "time.c",
    "queue.c",
    NULL
}};

static BuildSubdir persistobj = { "persist", {
    "ioctx.c", "file.c", "session.c","memkeyed.c",
    NULL
}};

static BuildSubdir parserobj = {"parser", {
    "match.c", "roebling.c", "scursor.c", "range.c", NULL
}};

static BuildSubdir formatsobj = {"formats", {
    "xml.c", "xml_parser.c", "http_parser.c", "oset.c", "oset_parser.c",
    NULL
}};

static BuildSubdir testobj = { "tests", {
    "utils.c", "tests.c", "span_setup_tests.c","core_tests.c", "string_tests.c", "table_tests.c",
    "match_tests.c", "span_tests.c", "hash_tests.c", "roebling_tests.c",
    "xml_tests.c", "http_tests.c", "serve_tests.c", "queue_tests.c",
    "mock_109strings.c", "ioctx_tests.c", "oset_tests.c",
    NULL
}};

static BuildSubdir serveobj = { "serve", {
    "serve.c", "proto.c", "req.c", "http_proto.c", "handler.c", "linux.c",
    NULL
}};

static BuildSubdir appsobj = { "apps", {
    "appsdebug.c", "example.c",
    NULL
}};

BuildSubdir *allobj[] = {&coreobj, &testobj, &parserobj, &formatsobj, &persistobj, &serveobj, &appsobj, NULL};
