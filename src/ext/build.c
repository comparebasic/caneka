#include <external.h>
#include <caneka.h>
#include <builder.h>

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
    /*
    "./LICENCE",
    NULL
    */
    {
        NULL,
        NULL,
        NULL
    }
};

char *sources[] = {
    "types/",
    "doc/",
    "navigate/",
#ifdef CRYPTO
    "auth/",
#endif
    "parser/",
    "serve/",
    "transp.deprecated/",    
    "format/",
    "format/html/",
    "format/config/",
    "format/xml/",
    "persist/",
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
    ctx.version = "1.0-seel";
    ctx.dist = "build";
    ctx.src = "src/ext";
    ctx.targets = NULL;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = NULL;
    ctx.args.staticLibs = NULL;
    ctx.sources = sources;
    ctx.genConfigs = genConfigs;

    return (Build(&ctx) & ERROR) ? 2 : 0;
}
