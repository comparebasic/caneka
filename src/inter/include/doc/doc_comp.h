typedef Inst DocComp;
status DocComp_Init(MemCh *m);
DocComp *DocComp_FromStr(MemCh *m, StrVec *src, StrVec *name);
void DocComp_FillFolder(DocComp *comp, StrVec *path);
