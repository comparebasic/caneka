#ifndef BUILDER_H
#define BUILDER_H

#define BUILDER_READ_SIZE 512

#include "detect.h"
#include <base_module.h>

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
        Str *barStart;
        Str *barLead;
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
    microTime start;
    microTime modified;
    StrVec *dir;
    StrVec *src;
    struct {
        StrVec *name;
        StrVec *target;
        StrVec *targetName;
        StrVec *version;
        StrVec *source;
        StrVec *dest;
        StrVec *binDest;
        Hashed *depKv;
        Span *staticlibs;
        Span *liblist;
        Span *inc;
    } current;
    struct {
        StrVec *buildDir;
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

status BuildCtx_LogOut(BuildCtx *ctx);
void BuildCtx_SetQuiet(boolean quiet);

status BuildCli_RenderStatus(MemCh *m, void *a);
status BuildCli_SetupComplete(BuildCtx *ctx);
status BuildCli_SetupStatus(BuildCtx *ctx);

status BuildCtx_ParseDependencies(BuildCtx *ctx, StrVec *key, StrVec *path);

status BuildCtx_GenAllIncSpan(BuildCtx *ctx);
status BuildCtx_GenInclude(BuildCtx *ctx, Span *modlist);
status BuildCtx_GenStrArr(BuildCtx *ctx, Span *files, Str *filter);
status BuildCtx_GenStr(BuildCtx *ctx, StrVec *file, Str *filter);

status BuildCtx_BuildModule(BuildCtx *ctx, StrVec *key, DirSelector *sel);
status BuildCtx_BuildObject(BuildCtx *ctx, StrVec *name, DirSelector *sel);
status BuildCtx_LinkObject(BuildCtx *ctx, StrVec *name, DirSelector *sel);

status BuildCtx_ToSInit(MemCh *m);

status BuildCtx_Build(BuildCtx *ctx);
BuildCtx *BuildCtx_Make(MemCh *m);

#endif
