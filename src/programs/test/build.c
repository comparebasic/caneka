#include <external.h>
#include <caneka.h>
#include <builder.h>

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
#ifdef INTER 
    "./build/libcnkinter/libcnkinter.a",
#endif
#ifdef CRYPTO_NACL 
    "./build/libcnknacl/libcnknacl.a",
    "/external/lib/libnacl.a",
#endif
#ifdef EXT
    "./build/libcnkext/libcnkext.a",
#endif
    "./build/libcaneka/libcaneka.a",
    NULL
};

static char *sources[] = {
    "testsuite.c",
    "fixtures/",
    "base/tests/",
#ifdef EXT
    "ext/tests/",
#endif
#ifdef CRYPTO_NACL 
    "third/crypto/tests/",
#endif
#ifdef INTER 
    "inter/tests/",
#endif
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
    ctx.args.libs = NULL;
    ctx.args.staticLibs = staticLibs;
    ctx.sources = sources;
    ctx.genConfigs = NULL;

    return (Build(&ctx, argc, argv) & ERROR) ? 2 : 0;
}
