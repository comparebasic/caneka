#include <external.h>
#include <caneka.h>
#include <builder.h>

#define TEST_REQ 1

/* parameters */

static Executable targets[] = {
    {NULL, NULL},
};

static char *cflags[] = {
        "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
        "-DINSECURE",
        "-DCNK_EXT",
        NULL
};

static char *inc[] = {
    "-I./artifact/src/include/",
    "-I./artifact/src/base/include/",
    "-I./artifact/src/ext/include/",
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
static BuildSubdir typesobj = { "types", {
    "init.c",
    "strings.c",
    "classdef.c",
    "object.c",
    "fetcher.c",
    "fetch_target.c",
    "types_cls.c",
    NULL
}};

static BuildSubdir docobj = { "doc", {
    "licences.c",
    NULL
}};

static BuildSubdir navigateobj = { "navigate", {
    "node.c",
    "mess.c",
    "relation.c",
    "compare.c",
    "frame.c",
    "queue.c",
    "queue_crit.c",
    "navigate_tos.c",
    "navigate_cls.c",
    NULL
}};

static BuildSubdir parserobj = { "parser", {
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

static BuildSubdir serveobj = { "serve", {
    "queue_crits.c",
    NULL
}};


static BuildSubdir sequenceobj = { "sequence", {
    "sequence.c",
    "sequence_cls.c",
    "table_utils.c",
    NULL
}};


static BuildSubdir transpobj = { "transp", {
    "transp.c",
    "transp_ctx.c",
    "transp_file.c",
    "transp_cls.c",
    NULL
}};

static BuildSubdir formatobj = { "format", {
    "format.c",
    NULL
}};

static BuildSubdir fmtobj = { "format/fmt", {
    "fmt.c",
    "fmt_roebling.c",
    "fmt_tokenize.c",
    "fmt_html.c",
    NULL
}};

static BuildSubdir xmlobj = { "format/xml", {
    "tag.c",
    NULL,
}};

static BuildSubdir persistobj = { "persist", {
    "persist_tos.c",
    "persist.c",
    "file.c",
    NULL
}};

static BuildSubdir *objdirs[] = {
    &typesobj,
    &docobj,
    &navigateobj,
    &sequenceobj,
    &serveobj,
    &parserobj,
    &fmtobj,
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
    ctx.libtarget = "libcnkext";
    ctx.version = "1.0-object-def";
    ctx.dist = "build";
    ctx.src = "artifact/src/ext";
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
