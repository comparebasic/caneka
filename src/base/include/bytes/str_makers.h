
#define Str_FuncName(m) Str_FromCstr((m), FUNCNAME, STRING_COPY)

Str *Str_MemCount(MemCh *m, i64 mem);
i64 Str_Trunc(Str *s, i64 amount);
i64 Str_AddMemCount(Str *s, i64 mem);
Str *Str_Prefixed(MemCh *m, Str *s, Str *prefix);
Str *Str_BuffFromCstr(MemCh *m, char *cstr);
status Str_AddChain(MemCh *m, Str *s, void *args[]);
Str *Str_ToUpper(MemCh *m, Str *s);
Str *Str_ToTitle(MemCh *m, Str *s);
Str *Str_ToUpper(MemCh *m, Str *s);
Str *Str_ToLower(MemCh *m, Str *s);
