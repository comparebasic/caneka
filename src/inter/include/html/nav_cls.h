typedef Object Nav;

#define Nav_MemCh(nav) ((nav)->order->m)

Nav *Nav_Make(MemCh *m);
status Nav_Add(Nav *nav, StrVec *path, void *a);
void *Nav_GetIndex(MemCh *m, FetchTarget *tg, void *item, void *source);
void *Nav_PathsIter(MemCh *m, FetchTarget *tg, void *item, void *source);
status Nav_ClsInit(MemCh *m);
status Nav_SetStatus(MemCh *m, void *a, status flag);
status Nav_UnSetStatusFunc(MemCh *m, void *a, status flag);
