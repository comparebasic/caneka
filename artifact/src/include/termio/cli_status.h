typedef struct cli_status {
    Type type;
    DoFunc render;
    Abstract *source;
    Span *lines;
    Span *tbl;
    i32 cols;
    i32 rows;
} CliStatus;

CliStatus *CliStatus_Make(MemCtx *m, DoFunc render, Abstract *source);
status CliStatus_Print(MemCtx *m, CliStatus *cli);
status CliStatus_PrintFinish(MemCtx *m, CliStatus *cli);
status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows);
status CliStatus_SetKey(MemCtx *m, CliStatus *cli, Str *key, struct int_pair *pair);
status CliStatus_SetByKey(MemCtx *m, CliStatus *cli, Str *key, Str *s);
