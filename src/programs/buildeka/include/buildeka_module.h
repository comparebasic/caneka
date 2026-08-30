#ifndef BUILDER_H
#define BUILDER_H

#define BUILDER_READ_SIZE 512

#include "detect.h"
#include <base_module.h>

#include "types/range.h"

enum cli_name_idx {
    BUILIDER_CLI_LIBFILENAME = 0,
    BUILIDER_CLI_ACTION,
    BUILIDER_CLI_SOURCE,
    BUILIDER_CLI_DEST,
};

enum mod_declare_idx {
    BUILD_MOD_DECLARE_TAG = 0,
    BUILD_MOD_DECLARE_LABEL = 1,
    BUILD_MOD_DECLARE_VALUE = 2,
};

enum build_types {
    BUILD_EXEC = 1 << 8,
    BUILD_STATIC = 1 << 9,
    BUILD_LINK = 1 << 10,
    BUILD_SKIP = 1 << 11,
    BUILD_CHOICE = 1 << 12,
    BUILD_INCLUDE = 1 << 13,
    BUILD_SHARED = 1 << 14,
    BUILD_SUB_DEP = 1 << 15,
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

typedef struct build_module {
    Type type;
    StrVec *name;
    StrVec *target;
    StrVec *targetName;
    StrVec *src;
    DirSel *sel;
} BuildModule;

typedef struct buildctx {
    Type type;
    MemCh *m;
    struct timespec start;
    struct timespec modified;
    StrVec *dir;
    StrVec *src;
    Ident *ident;
    Table *options;
    Table *deps;
    Node *config;
    struct {
        StrVec *key;
        StrVec *name;
        StrVec *target;
        StrVec *targetName;
        StrVec *version;
        StrVec *source;
        StrVec *dest;
        StrVec *binDest;
        Span *staticlibs;
        Span *liblist;
        Span *inc;
        Span *flags;
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
        Span *libDirs;
    } input;
    struct {
        Str *cc;
        Str *ccVersion;
        Str *ar;
    } tools;
    struct {
        Single *totalSources;
        Single *countSources;
        Single *totalModules;
        Single *countModules;
        Single *totalModuleSources;
        Single *countModuleSources;
    } metrics;
    struct {
        CliStatus *cli;
        BuildCliFields fields;
    } cli;
} BuildCtx;

void BuildCtx_Config(BuildCtx *ctx);
status BuildCtx_Build(BuildCtx *ctx);

status BuildCtx_Log(BuildCtx *ctx);
void BuildCtx_SetQuiet(boolean quiet);

status BuildCli_RenderStatus(MemCh *m, void *a);
status BuildCli_SetupComplete(BuildCtx *ctx);
status BuildCli_SetupStatus(BuildCtx *ctx);

status BuildCtx_ParseDependencies(BuildCtx *ctx, StrVec *key, StrVec *path);

status BuildCtx_GenAllIncSpan(BuildCtx *ctx);
status BuildCtx_GenIncFlags(BuildCtx *ctx, Span *modlist, Span *apis, Table *genlist);
status BuildCtx_GenStrArr(BuildCtx *ctx, Span *files, Str *filter);
status BuildCtx_GenStr(BuildCtx *ctx, StrVec *file, Str *filter);

status BuildCtx_BuildModule(BuildCtx *ctx, StrVec *name, DirSel *sel);
status BuildCtx_BuildObject(BuildCtx *ctx, StrVec *name, DirSel *sel);
status BuildCtx_LinkObject(BuildCtx *ctx, StrVec *name, DirSel *sel);
status BuildCtx_SetFlag(BuildCtx *ctx, StrVec *flag);
StrVec *BuildCtx_DestFromSrc(BuildCtx *ctx,
        StrVec *path, StrVec *src, StrVec *dest);

status BuildCtx_ToSInit(MemCh *m);

BuildCtx *BuildCtx_Make(MemCh *m);

BuildModule *BuildModule_Make(MemCh *m, BuildCtx *ctx, StrVec *name);
BuildModule *BuildModule_FromIdent(MemCh *m, BuildCtx *ctx, Ident *ident);

#endif
