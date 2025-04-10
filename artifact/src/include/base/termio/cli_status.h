status CliStatus_SetKey(MemCh *m, CliStatus *cli, Str *key, IntPair *pair);
status CliStatus_SetByKey(MemCh *m, CliStatus *cli, Str *key, Str *s);
status CliStatus_Print(MemCh *m, CliStatus *cli);
status CliStatus_PrintFinish(MemCh *m, CliStatus *cli);
status CliStatus_SetDims(CliStatus *cli, i32 cols, i32 rows);
CliStatus *CliStatus_Make(MemCh *m, DoFunc render, Abstract *source);
