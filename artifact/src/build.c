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
static BuildSubdir typesobj = { "ext/types", {
    "init.c",
    NULL
}};

static BuildSubdir navigateobj = { "ext/navigate", {
    "node.c",
    "mess.c",
    "navigate_tos.c",
    "relation.c",
    "compare.c",
    NULL
}};

static BuildSubdir parserobj = { "ext/parser", {
    "match.c",
    "match_replace.c",
    "patchar.c",
    "roebling.c",
    "parser.c",
    "snip.c",
    "tokenize.c",
    "parser_tos.c",
    "split.c",
    NULL
}};

static BuildSubdir transpobj = { "ext/transp", {
    "transp.c",
    "transp_ctx.c",
    "transp_tos.c",
    NULL
}};

static BuildSubdir formatobj = { "ext/format", {
    "format.c",
    NULL
}};

static BuildSubdir fmtobj = { "ext/format/fmt", {
    "fmt.c",
    "fmt_roebling.c",
    "fmt_tokenize.c",
    "fmt_html.c",
    NULL
}};

static BuildSubdir xmlobj = { "ext/format/xml", {
    "tag.c",
    NULL,
}};

static BuildSubdir persistobj = { "ext/persist", {
    "persist_tos.c",
    "persist.c",
    "file.c",
    NULL
}};

static BuildSubdir fixturesobj = { "tests/fixtures", {
    "mock_109strings.c",
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
    "clone_tests.c",
    "table_tests.c",
    "match_tests.c",
    "patchar_tests.c",
    "split_tests.c",
    "roebling_tests.c",
    "snip_tests.c",
    "cursor_tests.c",
    "mess_tests.c",
    "relation_tests.c",
    "iter_tests.c",
    "binpin_tests.c",
    "file_tests.c",
    "fmthtml_tests.c",
    "debugstack_tests.c",
    NULL
}};


static BuildSubdir *objdirs[] = {
    &typesobj,
    &navigateobj,
    &parserobj,
    &testsobj,
    &fixturesobj,
    &fmtobj,
    &formatobj,
    &persistobj,
    &transpobj,
    &xmlobj,
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
