#include <external.h>
#include <caneka.h>
#include <builder.h>

#define TEST_REQ 1

/* parameters */

static Executable targets[] = {
    {NULL, NULL},
};

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

static char *libs[] = {
    NULL
};

static char *staticLibs[] = {
    NULL
};

static BuildSubdir typesobj = { "types", {
    "strings.c",
    "init.c",
    NULL
}};


static BuildSubdir templobj = { "templ", {
    "templ.c",
    "templ_ctx.c",
    "templ_jump.c",
    "templ_roebling.c",
    "templ_cls.c",
    NULL
}};

static BuildSubdir wwwobj = { "www", {
    "route.c",
    "webserver.c",
    "nav.c",
    NULL
}};

static BuildSubdir httpobj = { "http", {
    "http_ctx.c",
    "http_task.c",
    "http_roebling.c",
    "http_cls.c",
    NULL
}};

static BuildSubdir *objdirs[] = {
    &typesobj,
    &templobj,
    &httpobj,
    &wwwobj,
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
    ctx.args.libs = libs;
    ctx.args.staticLibs = staticLibs;
    ctx.args.licenceFiles = licences;
    ctx.objdirs = (BuildSubdir **)objdirs;
    ctx.genConfigs = NULL;

    return (Build(&ctx) & ERROR) ? 2 : 0;
}
