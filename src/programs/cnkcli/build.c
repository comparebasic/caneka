#include <external.h>
#include <caneka.h>
#include <builder.h>

static Executable targets[] = {
    {"cnkcli", "main.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    "-DCNK_EXT",
    "-DCNK_INTER",
    "-DCNK_CRYPTO",
    NULL
};

static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
    "-I./src/inter/include/",
    "-I./src/third/api/include/",
    "-I./src/programs/cnkcli/include/",
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

static char *sources[] = {
    "unlog.c",
    "translate.c",
    "show.c",
    NULL,
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
    ctx.libtarget = "libcnkcli";
    ctx.version = NULL;
    ctx.dist = "build";
    ctx.src = "src/programs/cnkcli/";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = NULL;
    ctx.args.staticLibs = staticLibs;
    ctx.sources = sources;
    ctx.genConfigs = NULL;

    Build(&ctx);

    return 0;
}
