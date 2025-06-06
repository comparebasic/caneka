#include <external.h>
#include <caneka.h>
#include <cnkbuild.h>

#define TEST_REQ 1

/* parameters */

static Executable targets[] = {
    {"testreq", "programs/testreq.c"},
    {"caneka", "programs/main.c"},
    {NULL, NULL},
};

static char *cflags[] = {
        "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
        "-DINSECURE",
        NULL
};
static char *inc[] = {
    "-I./artifact/src/include/",
    NULL
};
static char *libs[] = {
    "-lcrypto",
    NULL
};

/* sources */
static BuildSubdir typesobj = { "types", {
    "init.c",
    NULL
}};

static BuildSubdir navigateobj = { "navigate", {
    "node.c",
    "mess.c",
    "navigate_tos.c",
    "relation.c",
    "compare.c",
    NULL
}};

static BuildSubdir parserobj = { "parser", {
    "match.c",
    "patchar.c",
    "roebling.c",
    "parser.c",
    "snip.c",
    "tokenize.c",
    "parser_tos.c",
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

static BuildSubdir cryptoobj = { "crypto", {
    "asymetric.c",
    "bio.c",
    "crypto_api.c",
    "crypto_debug.c",
    "digest.c",
    "enc_pair.c",
    "key.c",
    "salty.c",
    "secure.c",
    "symetric.c",
    NULL
}};

static BuildSubdir fmtobj = { "format/fmt", {
    "fmt.c",
    "fmt_roebling.c",
    "fmt_tokenize.c",
    NULL
}};


static BuildSubdir xmlobj = { "format/xml", {
    "xml.c",
    "xml_parser.c",
    NULL,
}};

static BuildSubdir cashobj = { "format/cash", {
    "cash.c",
    "cash_rbl.c",
    NULL,
}};

static BuildSubdir kveobj = { "format/kve", {
    "kve_capture.c",
    "kve_parser.c",
    "kve_from.c",
    "kve_to.c",
    NULL,
}};

static BuildSubdir termiofmtobj = { "format/termio", {
    "termio_parser.c",
    NULL,
}};

static BuildSubdir osetobj = { "format/oset", {
    "oset.c",
    "oset_from.c",
    "oset_item.c",
    "oset_parser.c",
    "oset_to.c",
    NULL,
}};

static BuildSubdir rblshobj = { "format/cnkrbl", {
    "rbl_parser_defs.c",
    "rblsh_capture.c",
    "rblsh_cnf.c",
    "rblsh_items.c",
    "rblsh_syntax.c",
    NULL,
}};

static BuildSubdir httpobj = { "format/http", {
    "http_parser.c",
    NULL
}};

static BuildSubdir cdocobj = { "format/cdoc", {
    "cdoc_capture.c",
    "cdoc_ctx.c",
    "cdoc_parser.c",
    "cdoc_utils.c",
    NULL
}};

static BuildSubdir canekaobj = { "format/caneka", {
    "caneka.c",
    "caneka_ctx.c",
    "caneka_module.c",
    "caneka_parser.c",
    "caneka_populate.c",
    "caneka_to.c",
    NULL
}};

static BuildSubdir xmltobj = { "lang/xmlt", {
    "xmlt.c",
    "xmlt_template.c",
    NULL
}};

static BuildSubdir persistobj = { "persist", {
    "auth.c",
    "file.c",
    "ioctx.c",
    "memlocal.c",
    "path.c",
    "session.c",
    "single_tolocal.c",
    "spool.c",
    "tolocal",
    "user.c",
    NULL
}};

static BuildSubdir serveobj = { "serve", {
    "handler.c",
    "http_proto.c",
    "io_proto.c",
    "sub_proto.c",
    "linux.c",
    "proto.c",
    "procioset.c",
    "req.c",
    "serve.c",
    "serve_debug.c",
    NULL
}};


static BuildSubdir fixturesobj = { "tests/fixtures", {
    "mock_109strings.c",
    /*
    "cycleserve_example.c",
    "utils.c",
    */
    NULL,
}};

static BuildSubdir testsobj = { "tests/tests", {
    "core_tests.c",
    "memch_tests.c",
    "span_tests.c",
    "str_tests.c",
    "strvec_tests.c",
    "stream_tests.c",
    "blank_test.c",
    "hash_tests.c",
    "table_tests.c",
    "match_tests.c",
    "patchar_tests.c",
    "roebling_tests.c",
    "snip_tests.c",
    "cursor_tests.c",
    "mess_tests.c",
    "relation_tests.c",
    "iter_tests.c",
    "binpin_tests.c",
    /*
    "crypto_tests.c",
    "http_tests.c",
    "ioctx_tests.c",
    "iter_str_tests.c",
    "memlocal_tests.c",
    "nestedd_tests.c",
    "oset_tests.c",
    "roebling_syntax_tests.c",
    "salty_tests.c",
    "serve_tests.c",
    "textfilter_tests.c",
    "termio_format_tests.c",
    "user_tests.c",
    "xml_tests.c",
    "xmlt_template_tests.c",
    "crypto_key_tests.c",
    "access_tests.c",
    */
    NULL
}};


static BuildSubdir *objdirs[] = {
    &typesobj,
    &navigateobj,
    &parserobj,
    &testsobj,
    &fixturesobj,
    /*
    &transpobj,
    &cryptoobj,
    &cashobj,
    &kveobj,
    &osetobj,
    &xmlobj,
    &rblshobj,
    */
    &fmtobj,
    /*
    &termiofmtobj,
    &httpobj,
    &cdocobj,
    &canekaobj,
    &xmltobj,
    &persistobj,
    &serveobj,
    */
    NULL
};

int main(int argc, char **argv){
    if(MemBook_Make(NULL) == NULL){
        Fatal(FUNCNAME, FILENAME, LINENUMBER, "Unable to allocate Mem_Book", NULL);
        exit(1);
    };
    MemCh *m = MemCh_Make();
    Caneka_InitBase(m);
    BuildCtx ctx;
    BuildCtx_Init(m, &ctx);

    ctx.tools.cc = "clang";
    ctx.tools.ar = "ar";
    ctx.libtarget = "libcaneka";
    ctx.dist = "build";
    ctx.src = "artifact/src";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.objdirs = (BuildSubdir **)objdirs;

    Build(&ctx);

    return 0;
}
