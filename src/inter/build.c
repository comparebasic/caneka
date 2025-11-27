#include <external.h>
#include <caneka.h>
#include <builder.h>

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    "-DCNK_EXT", "-DCNK_INTER",
    NULL
};

static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
    "-I./src/inter/include/",
    "-I./build/include/",
    NULL
};

char *sources[] = {
    "session/",
    "http/",
    "www/",
    "templ/",
    "types/",
    NULL
};

/*
char *genConfigs[] = {
    "./LICENCE",
    NULL
};
*/

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
    ctx.libtarget = "libcnkinter";
    ctx.version = "1.0-inter";
    ctx.dist = "build";
    ctx.src = "src/inter";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = NULL;
    ctx.args.staticLibs = NULL;
    ctx.sources = sources;
    ctx.genConfigs = NULL;

    return (Build(&ctx) & ERROR) ? 2 : 0;
}
