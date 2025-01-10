#include "mini/builder.h"

#define TEST_REQ 1

/* configuration */
#define CC "clang"
#define AR "ar"
#define CFLAGS cflags
#define INC inc 
#define LIBS libs
#define TARGETS targets 
#define LIBTARGET "libcaneka"
#define MAIN "main.c"
#define ALL allobj
#define VERBOSE 2

static Target targets[] = {
    {"testreq", "tests/testreq.c"},
    {"caneka", "main.c"},
    {"formatter", "formatter.c"},
    {"loopit", "loopit.c"},
    {"rblsh", "rblsh.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    "-DDEBUG_STACK",
    NULL
};

static char *inc[] = {
    "-I./dist/src/include/", "-I./dist/src/apps/include/",
    NULL
};

static char *libs[] = {
    NULL
};

static BuildSubdir memobj = { "mem", {
    "mem.c",
    NULL
}};

static BuildSubdir coreobj = { "core", {
    "core.c",
    "debug.c",
    "debug_stack.c",
    "error.c",
    "guard.c",
    "log.c",
    "testsuite.c",
    NULL
}};

static BuildSubdir cryptoobj = { "crypto", {
    "crypto_api.c",
    "enc_pair.c",
    "salty.c",
    "sane.c",
    "sha256.c",
    NULL
}};

static BuildSubdir formatsobj = { "formats", {
    "cash.c",
    "cash_rbl.c",
    "http_parser.c",
    "oset.c",
    "oset_from.c",
    "oset_item.c",
    "oset_parser.c",
    "oset_to.c",
    "rblsh.c",
    "xml.c",
    "xml_parser.c",
    NULL
}};

static BuildSubdir langobj = { "lang", {
    "caneka.c",
    "caneka_ctx.c",
    "caneka_module.c",
    "caneka_parser.c",
    "caneka_populate.c",
    "caneka_to.c",
    "cnk_roebling_cnf.c",
    "cnk_roebling_items.c",
    "cnk_roebling_syntax.c",
    "fmt_ctx.c",
    "fmt_def.c",
    "fmt_html.c",
    "fmt_item.c",
    "human_present.c",
    "nestedd.c",
    "rbl_parser_defs.c",
    "transp.c",
    "xmlt.c",
    "xmlt_template.c",
    NULL
}};

static BuildSubdir parserobj = { "parser", {
    "match.c",
    "patchar.c",
    "roebling.c",
    NULL
}};

static BuildSubdir persistobj = { "persist", {
    "access.c",
    "auth.c",
    "dir.c",
    "file.c",
    "ioctx.c",
    "memlocal.c",
    "session.c",
    "spool.c",
    "subprocess.c",
    "user.c",
    NULL
}};

static BuildSubdir secureobj = { "secure", {
    "phrase.c",
    NULL
}};

static BuildSubdir sequenceobj = { "sequence", {
    "array.c",
    "chain.c",
    "iter.c",
    "lookup.c",
    "mess.c",
    "queue.c",
    "slab.c",
    "span.c",
    "spandef.c",
    "spanquery.c",
    "table.c",
    "table_chain.c",
    NULL
}};

static BuildSubdir serveobj = { "serve", {
    "handler.c",
    "http_proto.c",
    "io_proto.c",
    "linux.c",
    "proto.c",
    "req.c",
    "serve.c",
    NULL
}};

static BuildSubdir stringobj = { "string", {
    "cursor.c",
    "iter_str.c",
    "string.c",
    "string_equals.c",
    "string_makers.c",
    "string_utils.c",
    "strray.c",
    "strsnip.c",
    NULL
}};

static BuildSubdir testsobj = { "tests", {
    "blank_test.c",
    "core_tests.c",
    "crypto_tests.c",
    "formatter.c",
    "hash_tests.c",
    "http_tests.c",
    "ioctx_tests.c",
    "iter_str_tests.c",
    "match_tests.c",
    "memlocal_tests.c",
    "mock_109strings.c",
    "nestedd_tests.c",
    "oset_tests.c",
    "patmatch_tests.c",
    "proto_http_tests.c",
    "queue_tests.c",
    "roebling_syntax_tests.c",
    "roebling_tests.c",
    "salty_tests.c",
    "serve_tests.c",
    "span_setup_tests.c",
    "span_tests.c",
    "string_tests.c",
    "table_tests.c",
    "testreq.c",
    "tests.c",
    "textfilter_tests.c",
    "user_tests.c",
    "utils.c",
    "xml_tests.c",
    "xmlt_template_tests.c",
    NULL
}};

static BuildSubdir utilobj = { "util", {
    "abstract.c",
    "blank.c",
    "boolean.c",
    "clone.c",
    "compare.c",
    "do.c",
    "hash.c",
    "int.c",
    "maker.c",
    "result.c",
    "single.c",
    "time.c",
    NULL
}};

static BuildSubdir appsobj = { "apps", {
    "appsdebug.c",
    "example.c",
    NULL
}};

BuildSubdir *allobj[] = {
    &memobj,
    &coreobj,
    &formatsobj,
    &langobj,
    &parserobj,
    &persistobj,
    &sequenceobj,
    &serveobj,
    &stringobj,
    &testsobj,
    &utilobj,
#ifdef CRYPTO
    &cryptoobj,
    &secureobj,
#endif
    &appsobj,
    NULL
};

#include "mini/builder.c"
