enum cdoc_types {
   _CDOC_START = _TYPE_APPS_END,
   CDOC_TARGET_FMT,
   CDOC_TARGET_HTML,
   CDOC_TARGET_MD,
   CDOC_START,
   CDOC_COMMENT,
   CDOC_WS,
   CDOC_TYPE,
   CDOC_THING,
   CDOC_END,
   _CDOC_END,
};

status Cdoc_Capture(word captureKey, int matchIdx, String *s, Abstract *source);
FmtCtx *CdocCtx_Make(MemCtx *m, Abstract *source);
Roebling *Cdoc_RblMake(MemCtx *m, FmtCtx *ctx);
status Cdoc_AddParsers(MemCtx *m, Span *parsers, Lookup *desc);
char *CdocRange_ToChars(word range);
