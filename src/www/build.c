#include <external.h>
#include <caneka.h>
#include <builder.h>

/* parameters */

static Executable targets[] = {
    {NULL, NULL},
};

static char *cflags[] = {
        "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
        "-DINSECURE",
        "-DCNK_EXT",
        "-DCNK_LANG",
        "-DCNK_WWW",
        NULL
};

static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
    "-I./src/lang/include/",
    "-I./src/www/include/",
    NULL
};

static char *libs[] = {
    NULL
};

static char *staticLibs[] = {
    NULL
};

/* sources */
static BuildSubdir html = { "html", {
    "nav_cls.c",
    "page_cls.c",
    NULL
}};

static BuildSubdir types = { "types", {
    "init.c",
    "strings.c",
    NULL
}};

static BuildSubdir routes = { "route", {
    "route.c",
    NULL
}};

static BuildSubdir *objdirs[] = {
    &html,
    &types,
    &routes,
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
    BuildCtx ctx;
    BuildCtx_Init(m, &ctx);

    ctx.tools.cc = "clang";
    ctx.tools.ar = "ar";
    ctx.libtarget = "libcnkwww";
    ctx.version = "1.strvec-alpha";
    ctx.dist = "build";
    ctx.src = "src/www";
    ctx.targets = (Executable *)targets;
    ctx.args.cflags = cflags;
    ctx.args.inc = inc;
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = licences;
    ctx.objdirs = (BuildSubdir **)objdirs;
    ctx.genConfigs = NULL;

    return (Build(&ctx) & ERROR) ? 2 : 0;
}
