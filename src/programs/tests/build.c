#include <external.h>
#include <caneka.h>
#include <builder.h>

/* parameters */

static Executable targets[] = {
    {"tests", "main.c"},
    {"content-fetch", "content-fetch.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    "-DINSECURE",
    "-DCNK_EXT",
    "-DCNK_INTER",
    "-DCNK_CRYPTO",
    /*
    "-DMEMTEST",
    */
    NULL
};

static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
    "-I./src/inter/include/",
    "-I./src/third/api/include/",
    "-I./src/programs/tests/include/",
    NULL
};

static char *staticLibs[] = {
    "./build/libcnkinter/libcnkinter.a",
    "./build/libcnknacl/libcnknacl.a",
    "/external/lib/libnacl.a",
    "./build/libcnkext/libcnkext.a",
    "./build/libcaneka/libcaneka.a",
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
    "binseg_tests.c",
    "buff_tests.c",
    "clone_tests.c",
    "config_tests.c",
    "core_tests.c",
    "crypto_tests.c",
    "cursor_tests.c",
    "debugstack_tests.c",
    "encoding_tests.c",
    "filedb_tests.c",
    "fmthtml_tests.c",
    "hash_tests.c",
    "iter_tests.c",
    "match_tests.c",
    "memch_tests.c",
    "mess_tests.c",
    "object_tests.c",
    "patchar_tests.c",
    "path_tests.c",
    "queue_tests.c",
    "relation_tests.c",
    "roebling_tests.c",
    "route_tests.c",
    "servetcp_tests.c",
    "snip_tests.c",
    "span_tests.c",
    "split_tests.c",
    "stash_tests.c",
    "str_tests.c",
    "stream_tests.c",
    "strvec_tests.c",
    "table_tests.c",
    "taskstep_tests.c",
    "templ_tests.c",
    "time_tests.c",
    "nodeobj_tests.c",
    "inet_tests.c",
    "parity_tests.c",
    "http_tests.c",
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
    Core_Direct(m, 1, 2);
    BuildCtx ctx;
    BuildCtx_Init(m, &ctx);

    ctx.tools.cc = "clang";
    ctx.tools.ar = "ar";
    ctx.libtarget = "libcnktest";
    ctx.version = NULL;
    ctx.dist = "build";
    ctx.src = "src/programs/tests";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = NULL;
    ctx.objdirs = (BuildSubdir **)objdirs;
    ctx.genConfigs = NULL;

    return (Build(&ctx) & ERROR) ? 2 : 0;
}
