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
    NULL
};

static char *inc[] = {
    "-I./artifact/src/include/",
    "-I./artifact/src/programs/tests/include/",
    NULL
};

static char *staticLibs[] = {
    "./build/libcnkbase/libcnkbase.a",
    "./build/libcaneka/libcaneka.a",
    NULL
};

static char *libs[] = {
    "-lcrypto",
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
    "ordtable_tests.c",
    "templ_tests.c",
    "nested_tests.c",
    "path_tests.c",
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
