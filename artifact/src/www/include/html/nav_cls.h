/* Nav < PathTable */ 

typedef Single Nav;

#define Nav_MemCh(nav) (((PathTable *)(((Single *)nav)->val.ptr))->tbl->m)

Nav *Nav_Make(MemCh *m);
status Nav_Add(Nav *nav, StrVec *path, Abstract *a);
Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
status Nav_ClsInit(MemCh *m);
status Nav_SetStatus(MemCh *m, Abstract *a, status flag);
status Nav_UnSetStatusFunc(MemCh *m, Abstract *a, status flag);
