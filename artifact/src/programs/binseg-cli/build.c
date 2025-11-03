#include <external.h>
#include <caneka.h>
#include <builder.h>

/* parameters */

static Executable targets[] = {
    {"binseg-cli", "main.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
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
    "-I./artifact/src/programs/binseg-cli/include/",
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

static BuildSubdir obj = { "components", {
    "unlog.c",
    "translate.c",
    "show.c",
    NULL,
}};


static BuildSubdir *objdirs[] = {
    &obj,
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
    ctx.libtarget = "libcnkbsgcli";
    ctx.version = NULL;
    ctx.dist = "build";
    ctx.src = "artifact/src/programs/binseg-cli/";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = NULL;
    ctx.objdirs = (BuildSubdir **)objdirs;
    ctx.genConfigs = NULL;

    Build(&ctx);

    return 0;
}
