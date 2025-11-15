
#define Str_FuncName(m) Str_FromCstr((m), FUNCNAME, STRING_COPY)

Str *Str_FromI64(MemCh *m, i64 i);
i64 Str_I64OnBytes(byte **_b, i64 i);
i64 Str_AddI64(Str *s, i64 i);
Str *Str_MemCount(MemCh *m, i64 mem);
i64 Str_Trunc(Str *s, i64 amount);
i64 Str_AddMemCount(Str *s, i64 mem);
Str *Str_Prefixed(MemCh *m, Str *s, Str *prefix);
Str *Str_BuffFromCstr(MemCh *m, char *cstr);
i64 Str_AddIByte(Str *s, byte i);
status Str_AddChain(MemCh *m, Str *s, void *args[]);
