typedef struct cli_status {
    Type type;
    DoFunc render;
    Abstract *source;
    Span *lines;
    Span *tbl;
    i32 cols;
    i32 rows;
} CliStatus;

status CliStatus_SetKey(MemCh *m, CliStatus *cli, Str *key, IntPair *pair);
status CliStatus_SetByKey(MemCh *m, CliStatus *cli, Str *key, Abstract *a);
Abstract *CliStatus_GetByKey(MemCh *m, CliStatus *cli, Str *key);
status CliStatus_Print(Stream *sm, CliStatus *cli);
status CliStatus_PrintFinish(Stream *sm, CliStatus *cli);
status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows);
CliStatus *CliStatus_Make(MemCh *m, DoFunc render, Abstract *source);
