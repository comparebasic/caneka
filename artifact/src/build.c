#include <external.h>
#include <caneka.h>
#include <cnkbuild.h>

#define TEST_REQ 1

/* parameters */

static Executable targets[] = {
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

static char *staticLibs[] = {
    NULL
};

/* sources */
static BuildSubdir typesobj = { "ext/types", {
    "init.c",
    "from.c",
    "atttable.c",
    NULL
}};

static BuildSubdir navigateobj = { "ext/navigate", {
    "node.c",
    "mess.c",
    "navigate_tos.c",
    "relation.c",
    "compare.c",
    "cash_ctx.c",
    "ordtable.c",
    "templ.c",
    "templ_jump.c",
    "nested.c",
    "frame.c",
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

static BuildSubdir sequenceobj = { "ext/sequence", {
    "sequence.c",
    "sequence_atts.c",
    NULL
}};


static BuildSubdir transpobj = { "ext/transp", {
    "transp.c",
    "transp_ctx.c",
    "transp_file.c",
    "transp_tos.c",
    "transp_atts.c",
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

static BuildSubdir cashobj = { "ext/format/cash", {
    "cash_roebling.c",
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

static BuildSubdir *objdirs[] = {
    &typesobj,
    &navigateobj,
    &sequenceobj,
    &parserobj,
    &fmtobj,
    &cashobj,
    &formatobj,
    &persistobj,
    &transpobj,
    &xmlobj,
    NULL
};

char *licences[] = {
    "./LICENCE",
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
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = licences;
    ctx.objdirs = (BuildSubdir **)objdirs;

    Build(&ctx);

    return 0;
}
