#include <external.h>
#include <caneka.h>
#include <cnkbuild.h>

static Executable targets[] = {
    {"doc", "main.c"},
    {NULL, NULL},
};

static char *cflags[] = {
        "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
        NULL
};
static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
    "-I./src/www/include/",
    "-I./src/programs/tests/include/",
    NULL
};

static char *staticLibs[] = {
    "./build/libcaneka/libcaneka.a",
    "./build/libcnkext.a/libcnkext.a",
    "./build/libcnklang.a/libcnklang.a",
    "./build/libcnkwww/libcnkwww.a",
    NULL
};

static char *libs[] = {
    NULL
};

/* sources */
static BuildSubdir obj = { "", {
    "doc_ctx.c",
    NULL
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
    ctx.libtarget = "libdoc";
    ctx.dist = "build";
    ctx.src = "src/programs/doc";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.objdirs = (BuildSubdir **)objdirs;

    Build(&ctx);

    return 0;
}
