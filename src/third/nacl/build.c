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
    "-DCNK_CRYPTO",
    "-DCNK_NACL",
    NULL
};

static char *inc[] = {
    "-I./src/include/",
    "-I./src/base/include/",
    "-I./src/ext/include/",
    "-I./src/third/api/include/",
    "-I./src/third/nacl/include/",
    "-I/external/include/",
    NULL
};

static char *staticLibs[] = {
    "./build/libcaneka/libcaneka.a",
    "./build/libcnkext/libcnkext.a",
    NULL
};

static char *libs[] = {
    NULL
};

static BuildSubdir naclobj = { "nacl", {
    "crypto_nacl.c",
    "crypto_hash-sha256-ref-hash.c",
    "crypto_sign-edwards25519sha512batch-ref-sign.c",
    NULL,
}};

static BuildSubdir *objdirs[] = {
    &naclobj,
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
    ctx.libtarget = "libcnknacl";
    ctx.version = NULL;
    ctx.dist = "build";
    ctx.src = "src/third/nacl/";
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
