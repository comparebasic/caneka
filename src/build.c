#include "include/builder.h"

#define TEST_REQ 1

/* configuration */
#define CC "clang"
#define AR "ar"
#define CFLAGS cflags
#define INC inc 
#define LIBS libs
#define BINARY "caneka"
#define MAIN "main.c"
#define ALL allobj
#define VERBOSE 2

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    NULL
};

static char *inc[] = {
    "-I./src/include/", "-I./src/apps/include/",
    NULL
};

static char *libs[] = {
    NULL
};

static BuildSubdir coreobj = { "core", {
    "caneka.c", "mem.c",  "string.c", "error.c",
	"log.c", "spandef.c", "span.c", "spanquery.c", "slab.c", "lookup.c", "chain.c", "table_chain.c",
    "testsuite.c", "hash.c", "table.c", "compare.c", 
    "iter.c", "array.c", "int.c", "maker.c", "do.c", 
    "mess.c", "single.c", "debug.c", "abstract.c", "time.c",
    "queue.c", "boolean.c",
    NULL
}};

static BuildSubdir persistobj = { "persist", {
    "ioctx.c", "file.c", "session.c","memlocal.c", "access.c",
    NULL
}};

static BuildSubdir parserobj = {"parser", {
    "match.c", "roebling.c", "scursor.c", "range.c", NULL
}};

static BuildSubdir formatsobj = {"formats", {
    "xml.c", "xml_parser.c", "http_parser.c", "oset.c", "oset_parser.c", "oset_to.c","oset_from.c", "oset_item.c", "cash.c",
    NULL
}};

static BuildSubdir langobj = {"lang", {
    "xmlt.c", "xmlt_template.c", "nestedd.c", "human_present.c",
    "fmt_ctx.c","fmt_item.c",
    NULL
}};

static BuildSubdir testobj = { "tests", {
    "utils.c", "tests.c", "span_setup_tests.c","core_tests.c", "string_tests.c", "table_tests.c",
    "match_tests.c", "span_tests.c", "hash_tests.c", "roebling_tests.c",
    "xml_tests.c", "http_tests.c", "serve_tests.c", "queue_tests.c",
    "mock_109strings.c", "ioctx_tests.c", "oset_tests.c",
    "memlocal_tests.c", "blank_test.c", "nestedd_tests.c", "xmlt_template_tests.c", "textfilter_tests.c", "crypto_tests.c", "salty_tests.c",
    NULL
}};

static BuildSubdir cryptoobj = { "crypto", {
    "sane.c", "sha256.c", "crypto_api.c", "enc_pair.c", "salty.c",
    NULL
}};

static BuildSubdir secureobj = { "secure", {
    "phrase.c",
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

BuildSubdir *allobj[] = {
    &coreobj, &testobj, &parserobj, &formatsobj, &persistobj,
#ifdef CRYPTO
    &cryptoobj,
    &secureobj,
#endif
    &serveobj, &appsobj, &langobj,
    NULL
};

#include "builder.c"
