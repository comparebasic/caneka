typedef struct cli_status {
    Type type;
    DoFunc render;
    Abstract *source;
    Span *lines;
    i32 cols;
    i32 rows;
} CliStatus;

CliStatus *CliStatus_Make(MemCtx *m, DoFunc render, Abstract *source);
status CliStatus_Print(MemCtx *m, CliStatus *cli);
status CliStatus_PrintFinish(MemCtx *m, CliStatus *cli);
status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows);
