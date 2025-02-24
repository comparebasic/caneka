typedef struct cli_status {
    Type type;
    DoFunc render;
    Abstract *source;
    Span *lines;
} CliStatus;

CliStatus *CliStatus_Make(MemCtx *m, DoFunc render, Abstract *source);
status CliStatus_Print(MemCtx *m, CliStatus *cli);
status CliStatus_PrintFinish(MemCtx *m, CliStatus *cli);
