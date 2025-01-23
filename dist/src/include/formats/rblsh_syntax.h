enum cnk_roebling_flags32 {
    CNK_RBL_FMT_FL_ADD = 1 << 16,
    CNK_RBL_FMT_FL_RESET = 1 << 17,
};

extern FmtCnf *cnk_roebling_configs;

FmtCtx *CnkRblCtx_Make(MemCtx *m, Abstract *source);
status CnkRoebling_AddParsers(MemCtx *m, Span *parsers, Lookup *marks);
status CnkRoebling_Capture(word captureKey, int matchIdx, String *s, Abstract *source);
Roebling *CnkRoeblingCtx_RblMake(MemCtx *m, FmtCtx *ctx);
FmtCtx *CnkRoeblingCtx_Make(MemCtx *m, Abstract *source);
status CnkRblLang_AddDefs(FmtCtx *ctx);

/* Trans functions */
Abstract *CnkRbl_Pat(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *key, Abstract *a);
Abstract *CnkRbl_PatClose(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a);
Abstract *CnkRbl_PatKeyOpen(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a);
Abstract *CnkRbl_Out(MemCtx *m, FmtDef *def, FmtCtx *fmt, String *s, Abstract *a);
