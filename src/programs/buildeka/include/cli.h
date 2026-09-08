enum cli_name_idx {
    BUILIDER_CLI_LIBFILENAME = 0,
    BUILIDER_CLI_ACTION,
    BUILIDER_CLI_SOURCE,
    BUILIDER_CLI_DEST,
};

typedef struct build_cli_fields {
    struct {
        Str *name; 
        Str *barStart;
        Str *barLead;
    } steps;
    void *current[5];
} BuildCliFields;

status BuildCli_SetupComplete(BuildCtx *ctx);
status BuildCli_SetupStatus(BuildCtx *ctx);
status BuildCli_Log(MemCh *m, void *a, void *source);
status BuildCli_RenderStatus(MemCh *m, void *a);
