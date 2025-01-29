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
#define DIST "build"
#define SRC "dist/src"

static Target targets[] = {
    {"testreq", "programs/testreq.c"},
    {"caneka", "programs/main.c"},
    {"formatter", "programs/formatter.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    NULL
};

static char *inc[] = {
    "-I./dist/src/include/",
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
    "error.c",
    "guard.c",
    "log.c",
    "testsuite.c",
    NULL
}};

static BuildSubdir debugobj = { "debug", {
    "debug.c",
    "debug_stack.c",
    "stepper.c",
    "stepper_funcs.c",
    "debug_typestrings.c",
    NULL,
}};

static BuildSubdir cryptoobj = { "crypto", {
    "crypto_api.c",
    "enc_pair.c",
    "salty.c",
    "sane.c",
    "sha256.c",
    NULL
}};

static BuildSubdir cashobj = { "formats/cash", {
    "cash.c",
    "cash_rbl.c",
    NULL,
}};

static BuildSubdir termiofmtobj = { "formats/termio", {
    "termio_parser.c",
    NULL,
}};

static BuildSubdir osetobj = { "formats/oset", {
    "oset.c",
    "oset_from.c",
    "oset_item.c",
    "oset_parser.c",
    "oset_to.c",
    NULL,
}};
static BuildSubdir xmlobj = { "formats/xml", {
    "xml.c",
    "xml_parser.c",
    NULL,
}};

static BuildSubdir rblshobj = { "formats/cnkrbl", {
    "rbl_parser_defs.c",
    "rblsh_capture.c",
    "rblsh_cnf.c",
    "rblsh_items.c",
    "rblsh_syntax.c",
    NULL,
}};

static BuildSubdir fmtobj = { "formats/fmt", {
    "fmt_html.c",
    NULL
}};

static BuildSubdir httpobj = { "formats/http", {
    "http_parser.c",
    NULL
}};

static BuildSubdir cdocobj = { "formats/cdoc", {
    "cdoc_capture.c",
    "cdoc_ctx.c",
    "cdoc_parser.c",
    "cdoc_utils.c",
    NULL
}};

static BuildSubdir canekaobj = { "formats/caneka", {
    "caneka.c",
    "caneka_ctx.c",
    "caneka_module.c",
    "caneka_parser.c",
    "caneka_populate.c",
    "caneka_to.c",
    NULL
}};

static BuildSubdir transpobj = { "transp", {
    "ext_match.c",
    "fmt_ctx.c",
    "fmt_def.c",
    "fmt_item.c",
    "fmt_resolver.c",
    "source.c",
    "target.c",
    "transp.c",
    NULL
}};

static BuildSubdir xmltobj = { "lang/xmlt", {
    "xmlt.c",
    "xmlt_template.c",
    NULL
}};

static BuildSubdir parserobj = { "parser", {
    "match.c",
    "patchar.c",
    "roebling.c",
    "parser_debug.c",
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
    "nestedd.c",
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
    "sub_proto.c",
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
    "strsnip.c",
    "strsnipstr.c",
    "string_debug.c",
    NULL
}};

static BuildSubdir termioobj = { "termio", {
    "rawmode.c",
    NULL
}};

static BuildSubdir testsobj = { "tests", {
    "tests.c",
    NULL
}};

static BuildSubdir testsuitesobj = { "tests/tests", {
    "blank_test.c",
    "core_tests.c",
    "crypto_tests.c",
    "hash_tests.c",
    "http_tests.c",
    "ioctx_tests.c",
    "iter_str_tests.c",
    "match_tests.c",
    "memctx_tests.c",
    "memlocal_tests.c",
    "nestedd_tests.c",
    "oset_tests.c",
    "queue_tests.c",
    "roebling_syntax_tests.c",
    "roebling_tests.c",
    "salty_tests.c",
    "serve_tests.c",
    "span_setup_tests.c",
    "span_tests.c",
    "string_tests.c",
    "strsnip_tests.c",
    "table_tests.c",
    "textfilter_tests.c",
    "user_tests.c",
    "xml_tests.c",
    "xmlt_template_tests.c",
    NULL
}};

static BuildSubdir testfixturesobj = { "tests/fixtures", {
    "cycleserve_example.c",
    "mock_109strings.c",
    "utils.c",
    NULL,
}};

static BuildSubdir utilobj = { "util", {
    "blank.c",
    "boolean.c",
    "clone.c",
    "compare.c",
    "do.c",
    "hash.c",
    "human_present.c",
    "int.c",
    "maker.c",
    "result.c",
    "single.c",
    "time.c",
    NULL
}};

BuildSubdir *allobj[] = {
    &memobj,
    &coreobj,
    &debugobj,
    &cryptoobj,
    &cashobj,
    &osetobj,
    &xmlobj,
    &rblshobj,
    &fmtobj,
    &termiofmtobj,
    &httpobj,
    &cdocobj,
    &canekaobj,
    &transpobj,
    &xmltobj,
    &parserobj,
    &persistobj,
    &secureobj,
    &sequenceobj,
    &serveobj,
    &stringobj,
    &testsobj,
    &termioobj,
    &testsuitesobj,
    &testfixturesobj,
    &utilobj,
    NULL
};

#include "mini/builder.c"
