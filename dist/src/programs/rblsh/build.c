#include "../../mini/builder.h"

/* configuration */
#define CC "clang"
#define AR "ar"
#define CFLAGS cflags
#define INC inc 
#define LIBS libs
#define TARGETS targets 
#define LIBTARGET "librblsh"
#define ALL allobj
#define VERBOSE 2
#define DIST "build"
#define SRC "dist/src/programs/rblsh"

static Target targets[] = {
    {"rblsh", "rblsh/rblsh.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    NULL
};

static char *inc[] = {
    "-I./dist/src/include/", "-I./dist/src/programs/rblsh/include/",
    NULL
};

static char *libs[] = {
    "build/libcaneka.a",
    NULL
};

static BuildSubdir obj = { ".", {
    "ctx.c",
    "debug.c",
    "parser.c",
    "req.c",
    NULL
}};

BuildSubdir *allobj[] = {
    &obj,
    NULL
};

#include "../../mini/builder.c"
