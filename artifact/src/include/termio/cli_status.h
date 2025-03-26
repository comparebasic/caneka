typedef struct cli_status {
    Type type;
    DoFunc render;
    Abstract *source;
    Span *lines;
    Span *tbl;
    i32 cols;
    i32 rows;
} CliStatus;

CliStatus *CliStatus_Make(MemCh *m, DoFunc render, Abstract *source);
status CliStatus_Print(MemCh *m, CliStatus *cli);
status CliStatus_PrintFinish(MemCh *m, CliStatus *cli);
status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows);
status CliStatus_SetKey(MemCh *m, CliStatus *cli, Str *key, struct int_pair *pair);
status CliStatus_SetByKey(MemCh *m, CliStatus *cli, Str *key, Str *s);
