typedef Inst DocComp;
status DocComp_Init(MemCh *m);
DocComp *DocComp_FromStr(MemCh *m, StrVec *src, Str *s);
status Doc_To(Buff *bf, DocComp *comp, ToSFunc func, TosFunc head, TosFunc foot);
