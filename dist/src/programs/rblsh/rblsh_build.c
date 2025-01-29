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

static Target targets[] = {
    {"rblsh", "rblsh/rblsh.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    NULL
};

static char *inc[] = {
    "-I./include/", "-I./programs/rblsh/include/",
    NULL
};

static char *libs[] = {
    "build/libcaneka.a",
    NULL
};

static BuildSubdir obj = { ".", {
    NULL
}};

BuildSubdir *allobj[] = {
    &obj,
    NULL
};

#include "../../mini/builder.c"
