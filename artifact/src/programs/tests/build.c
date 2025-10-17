#include <external.h>
#include <caneka.h>
#include <builder.h>

/* parameters */

static Executable targets[] = {
    {"tests", "main.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    "-DINSECURE",
    "-DCNK_EXT",
    "-DCNK_LANG",
    "-DCNK_WWW",
    "-DCNK_CRYPTO",
    NULL
};

static char *inc[] = {
    "-I./artifact/src/include/",
    "-I./artifact/src/base/include/",
    "-I./artifact/src/ext/include/",
    "-I./artifact/src/www/include/",
    "-I./artifact/src/lang/include/",
    "-I./artifact/src/third/api/include/",
    "-I./artifact/src/programs/tests/include/",
    NULL
};

static char *staticLibs[] = {
    "./build/libcaneka/libcaneka.a",
    "./build/libcnkext/libcnkext.a",
    "./build/libcnklang/libcnklang.a",
    "./build/libcnkwww/libcnkwww.a",
    "./build/libcnknacl/libcnknacl.a",
    "/external/lib/libnacl.a",
    NULL
};

static char *libs[] = {
    NULL
};

static BuildSubdir fixturesobj = { "fixtures", {
    "mock_109strings.c",
    NULL,
}};

static BuildSubdir testsobj = { "tests", {
    "core_tests.c",
    "memch_tests.c",
    "span_tests.c",
    "str_tests.c",
    "strvec_tests.c",
    "stream_tests.c",
    "blank_test.c",
    "hash_tests.c",
    "clone_tests.c",
    "encoding_tests.c",
    "table_tests.c",
    "stash_tests.c",
    "match_tests.c",
    "patchar_tests.c",
    "split_tests.c",
    "roebling_tests.c",
    "snip_tests.c",
    "cursor_tests.c",
    "mess_tests.c",
    "relation_tests.c",
    "iter_tests.c",
    "time_tests.c",
    "queue_tests.c",
    "file_tests.c",
    "fmthtml_tests.c",
    "debugstack_tests.c",
    "object_tests.c",
    "templ_tests.c",
    "templ_route_tests.c",
    "path_tests.c",
    "binseg_tests.c",
    "taskstep_tests.c",
    "servetcp_tests.c",
    "filedb_tests.c",
    "www_nav_tests.c",
    "route_tests.c",
    "crypto_tests.c",
    NULL
}};

static BuildSubdir *objdirs[] = {
    &testsobj,
    &fixturesobj,
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
    ctx.libtarget = "libcnktest";
    ctx.version = NULL;
    ctx.dist = "build";
    ctx.src = "artifact/src/programs/tests";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = NULL;
    ctx.objdirs = (BuildSubdir **)objdirs;

    Build(&ctx);

    return 0;
}
