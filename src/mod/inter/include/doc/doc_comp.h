typedef Inst DocComp;
status DocComp_Init(MemCh *m);
DocComp *DocComp_FromStr(MemCh *m, StrVec *src, StrVec *name);
Node *DocComp_DocOnly(MemCh *m, StrVec *name);
