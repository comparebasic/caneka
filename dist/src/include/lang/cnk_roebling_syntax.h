FmtCtx *CnkRblCtx_Make(MemCtx *m, Abstract *source);
status CnkRoebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks);
status CnkRoebling_Capture(word captureKey, int matchIdx, String *s, Abstract *source);
Roebling *CnkRoeblingCtx_RblMake(MemCtx *m, FmtCtx *ctx);
FmtCtx *CnkRoeblingCtx_Make(MemCtx *m, Abstract *source);
