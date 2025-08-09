/* Nav < Span */ 

typedef Single Nav;

#define Nav_Add(n, a) Span_Add(((Span *)(n)->val.ptr), (a))

Nav *Nav_Make(MemCh *m);
Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
status Nav_ClsInit(MemCh *m);
