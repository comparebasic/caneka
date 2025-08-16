/* Nav < OrdTable */ 

typedef Single Nav;

Nav *Nav_Make(MemCh *m);
status Nav_Add(Nav *nav, StrVec *path, Abstract *a);
Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
status Nav_ClsInit(MemCh *m);
