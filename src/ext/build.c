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
#ifdef CRYPTO
        "-DCNK_CRYPTO",
#endif
        NULL
};

static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
#ifdef CRYPTO
    "-I./src/third/api/include/",
#endif
    "-I./build/include/",
    NULL
};

static char *libs[] = {
    NULL
};


static char *saltGenSources[] = {"./examples/gensource/salt512", NULL};
static char *funnyGenSources[] = {"./examples/gensource/funny", NULL};
static GenConfig genConfigs[] = {
    {
        "salt.h", 
        "_#_",
        saltGenSources,
    },
    {
        "funny.h", 
        "_#_",
        funnyGenSources,
    },
    {
        NULL,
        NULL,
        NULL
    }
};

static char *staticLibs[] = {
    NULL
};

/* sources */
static BuildSubdir typesobj = { "types", {
    "strings.c",
    "init.c",
    "inst.c",
    "seel.c",
    "fetcher.c",
    "fetch_target.c",
    "types_tos.c",
    NULL
}};

static BuildSubdir docobj = { "doc", {
    "licence.c",
    NULL
}};

static BuildSubdir navigateobj = { "navigate", {
    "node.c",
    "nodeobj.c",
    "mess.c",
    "relation.c",
    "compare.c",
    "frame.c",
    "queue.c",
    "queue_crit.c",
    "queue_crit_time.c",
    "queue_crit_fds.c",
    "step.c",
    "task.c",
    "navigate_cls.c",
    NULL
}};

#ifdef CRYPTO
static BuildSubdir authobj = { "auth", {
    "auth_target.c",
    "auth_cred.c",
    "password.c",
    NULL
}};
#endif

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
    "proto_ctx.c",
    "serve_tcp.c",
    "tcp_ctx.c",
    "tcp_task.c",
    "serve_cls.c",
    NULL
}};

static BuildSubdir transpobj = { "transp.deprecated", {
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


static BuildSubdir htmlobj = { "format/html", {
    "html_escape_roebling.c",
    NULL
}};


static BuildSubdir fmtobj = { "format/fmt", {
    "fmt.c",
    "fmt_roebling.c",
    "fmt_tokenize.c",
    "fmt_html.c",
    NULL
}};

static BuildSubdir configobj = { "format/config", {
    "config_roebling.c",
    "config.c",
    NULL
}};

static BuildSubdir xmlobj = { "format/xml", {
    "tag.c",
    NULL,
}};

static BuildSubdir persistobj = { "persist", {
    "persist_tos.c",
    "persist.c",
    "binseg.c",
    NULL
}};

static BuildSubdir *objdirs[] = {
    &typesobj,
    &docobj,
    &navigateobj,
#ifdef CRYPTO
    &authobj,
#endif
    &serveobj,
    &parserobj,
    &fmtobj,
    &configobj,
    &formatobj,
    &persistobj,
    &transpobj,
    &xmlobj,
    &htmlobj,
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
    Core_Direct(m, 1, 2);
    BuildCtx ctx;
    BuildCtx_Init(m, &ctx);

    ctx.tools.cc = "clang";
    ctx.tools.ar = "ar";
    ctx.libtarget = "libcnkext";
    ctx.version = "1.0-object-def";
    ctx.dist = "build";
    ctx.src = "src/ext";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = licences;
    ctx.objdirs = (BuildSubdir **)objdirs;
    ctx.genConfigs = genConfigs;

    return (Build(&ctx) & ERROR) ? 2 : 0;
}
