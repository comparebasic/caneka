/* NavItem < TranspFile */ 
/* Nav < OrdTable */ 

Single *Nav_Make(MemCh *m, OrdTable *otbl);
Abstract *Nav_PathsIter(FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *Nav_BasePath(FetchTarget *tg, Abstract *item, Abstract *source);

Single *NavItem_Make(MemCh *m, TranspFile *tp);
Abstract *NavItem_Name(FetchTarget *tg, Abstract *item, Abstract *source);
Abstract *NavItem_Path(FetchTarget *tg, Abstract *item, Abstract *source);
status NavItem_ClsInit(MemCh *m);
