#ifndef BUILDER_H
#define BUILDER_H

#define BUILDER_READ_SIZE 512

typedef struct executable {
    char *bin;
    char *src;
} Executable;

enum current_idx {
    BUILIDER_CLI_LIBFILENAME = 0,
    BUILIDER_CLI_ACTION,
    BUILIDER_CLI_SOURCE,
    BUILIDER_CLI_DEST,
};

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
    Executable *targets;
    CliStatus *cli;
    BuildCliFields fields;
    char *dist;
    char *src;
    char *libtarget;
    char *version;
    struct {
        char *cc;
        char *ar;
    } tools;
    struct {
        char **inc;
        char **cflags;
        char **libs;
        char **staticLibs;
    } args;
    char **sources;
    GenConfig *genConfigs;
} BuildCtx;

status BuildCtx_Init(MemCh *m, BuildCtx *ctx);
status Build(BuildCtx *ctx);
status Generate(MemCh *m, Str *path, Str *key, char *varpaths[], Str *outpath);
#endif
