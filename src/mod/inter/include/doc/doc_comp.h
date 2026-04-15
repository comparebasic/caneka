typedef Inst DocComp;
status DocComp_Init(MemCh *m);
DocComp *DocComp_FromStr(MemCh *m, StrVec *src, StrVec *name);
NodeObj *DocComp_DocOnly(MemCh *m, StrVec *name);
