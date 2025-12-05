typedef struct cli_status {
    Type type;
    struct timespec tick;
    DoFunc render;
    void *source;
    Span *lines;
    Span *tbl;
    i32 cols;
    i32 rows;
} CliStatus;

status CliStatus_SetKey(MemCh *m, CliStatus *cli, Str *key, IntPair *pair);
status CliStatus_SetByKey(MemCh *m, CliStatus *cli, Str *key, void *a);
void *CliStatus_GetByKey(MemCh *m, CliStatus *cli, Str *key);
status CliStatus_Print(Buff *bf, CliStatus *cli);
status CliStatus_PrintFinish(Buff *bf, CliStatus *cli);
status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows);
CliStatus *CliStatus_Make(MemCh *m, DoFunc render, void *source);
