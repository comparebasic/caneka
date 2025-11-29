enum argflags {
    ARG_DEFAULT = 1 << 8,
    ARG_OPTIONAL = 1 << 9,
    ARG_MULTIPLE = 1 << 10,
    ARG_ABS_PATH = 1 << 11,
    ARG_CHOICE = 1 << 12,
};

typedef struct cli_args {
    Type type;
    MemCh *m;
    i32 argc;
    char **argv;
    Str *name;
    Table *resolve;
    Table *args;
    Span *explain;
} CliArgs;

status CharPtr_ToHelp(CliArgs *cli);
CliArgs *CliArgs_Make(i32 argc, char *argv[]);
status CliArgs_Parse(CliArgs *cli);
void *CliArgs_Get(CliArgs *cli, void *key);
StrVec *CliArgs_GetAbsPath(CliArgs *cli, void *key);
void CliArgs_Free(CliArgs *cli);

status Args_Add(CliArgs *cli, Str *key, void *value, word flags, StrVec *explain);
status Args_Init(MemCh *m);
