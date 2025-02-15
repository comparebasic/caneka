#include "../../mini/builder.h"

/* configuration */
#define CC "clang"
#define AR "ar"
#define CFLAGS cflags
#define INC inc 
#define LIBS libs
#define TARGETS targets 
#define LIBTARGET "libsigner"
#define ALL allobj
#define VERBOSE 2
#define DIST "build"
#define SRC "dist/src/programs/signer"

static Target targets[] = {
    {"signer", "signer.c"},
    {NULL, NULL},
};

static char *cflags[] = {
    "-g", "-Werror", "-Wno-incompatible-pointer-types-discards-qualifiers",
    "-DOPENSSL",
    NULL
};

static char *inc[] = {
    "-I./dist/src/include/", "-I./dist/src/programs/signer/include/",
    NULL
};

static char *libs[] = {
    "build/libcaneka.a",
    "-lssl",
    "-lcrypto",
    NULL
};

static BuildSubdir obj = { "", {
    "signer_parser.c",
    NULL
}};

BuildSubdir *allobj[] = {
    &obj,
    NULL
};

#include "../../mini/builder.c"
