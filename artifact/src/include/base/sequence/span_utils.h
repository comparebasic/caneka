i32 Span_Capacity(Span *p);
char **Span_ToCharArr(MemCh *m, Span *p);
status Span_SetRaw(Span *p, i32 idx, util *u);
util Span_GetRaw(Span *p, i32 idx);
status Span_Cull(Span *p, i32 count);
status Span_ReInit(Span *p);
Span *Span_Clone(MemCh *m, Span *p);
status Span_Add(Span *p, Abstract *t);
status Span_Wipe(Span *p);
