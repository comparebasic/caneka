enum cnk_roebling_flags32 {
    CNK_RBL_FMT_FL_ADD = 1 << 16,
    CNK_RBL_FMT_FL_RESET = 1 << 17,
};

FmtCtx *CnkRblCtx_Make(MemCtx *m, Abstract *source);
status CnkRoebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks);
status CnkRoebling_Capture(word captureKey, int matchIdx, String *s, Abstract *source);
Roebling *CnkRoeblingCtx_RblMake(MemCtx *m, FmtCtx *ctx);
FmtCtx *CnkRoeblingCtx_Make(MemCtx *m, Abstract *source);
status CnkRblLang_AddDefs(FmtCtx *ctx);
