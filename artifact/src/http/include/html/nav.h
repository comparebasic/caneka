/* NavItem < TranspFile */ 
/* Nav < Span */ 

Single *Nav_Make(MemCh *m, OrdTable *otbl);
Abstract *Nav_PathsIter(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *Nav_GetIndex(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);

Single *NavItem_Make(MemCh *m, TranspFile *tp);
Abstract *NavItem_Name(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *NavItem_Path(MemCh *m, FetchTarget *tg, Abstract *item, Abstract *source);
status NavItem_ClsInit(MemCh *m);
