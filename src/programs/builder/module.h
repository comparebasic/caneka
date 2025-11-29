#ifndef BUILDER_H
#define BUILDER_H

#include "../../base/module.h"

#define BUILDER_READ_SIZE 512

#if defined(__clang__)
    #define _gen_CC "clang"
    #define _gen_CC_VERSION __clang_major__
#elif defined(__INTEL_COMPILER)
    #define _gen_CC "icc"
    #define _gen_CC_VERSION __INTEL_COMPILER
#elif __GNUC__ 
    #define _gen_CC "gcc"
    #define _gen_CC_VERSION __GNUC__
#elif defined(_MS_VER)
    #define _gen_CC "msvc"
    #define _gen_CC_VERSION _MS_VER
#endif

#define _gen_AR "ar"

enum cli_name_idx {
    BUILIDER_CLI_LIBFILENAME = 0,
    BUILIDER_CLI_ACTION,
    BUILIDER_CLI_SOURCE,
    BUILIDER_CLI_DEST,
};

enum build_types {
    BUILD_EXEC = 1 << 8,
    BUILD_STATIC = 1 << 9,
    BUILD_LINK = 1 << 10,
    BUILD_SKIP = 1 << 11,
    BUILD_CHOICE = 1 << 12,
    BUILD_INCLUDE = 1 << 13,
};

typedef struct executable {
    char *bin;
    char *src;
} Executable;

typedef struct build_cli_fields {
    struct {
        Str *name; 
        Single *count;
        Single *total;
        Str *barStart; 
        Str *barEnd; 
    } steps;
    void *current[5];
} BuildCliFields;

typedef struct gen_config {
    char *file;
    char *key;
    char **args;
} GenConfig;

typedef struct buildctx {
    Type type;
    MemCh *m;
    util modified;
    StrVec *dir;
    StrVec *src;
    struct {
        StrVec *target;
        StrVec *targetName;
        StrVec *version;
        StrVec *source;
        StrVec *dest;
        Hashed *depKv;
    } current;
    struct {
        Span *inc;
        Span *cflags;
        Span *libs;
        Span *staticLibs;
        Span *sources;
        Span *objects;
        Span *gens;
        StrVec *srcPrefix;
        Table *dependencies;
        Span *libDirs;
        Single *totalSources;
        Single *countSources;
        Single *totalModules;
        Single *countModules;
        Single *totalModuleSources;
        Single *countModuleSources;
    } input;
    struct {
        Str *cc;
        Str *ccVersion;
        Str *ar;
    } tools;
    struct {
        CliStatus *cli;
        BuildCliFields fields;
    } cli;
} BuildCtx;

BuildCtx *BuildCtx_Make(MemCh *m);
status Build(BuildCtx *ctx, i32 argc, char *argv[]);
status Generate(MemCh *m, Str *path, Str *key, char *varpaths[], Str *outpath);
status LogOut(BuildCtx *ctx);

status BuildCli_RenderStatus(MemCh *m, void *a);
status BuildCli_SetupComplete(BuildCtx *ctx);
status BuildCli_SetupStatus(BuildCtx *ctx);
status BuildCtx_ToSInit(MemCh *m);

#endif
