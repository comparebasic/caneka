extern struct lookup *SeelLookup;
extern struct lookup *SeelNameLookup;

i32 Seel_GetIdx(Table *seel, void *key);
status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf);
status Seel_Init(MemCh *m);
void *Seel_ByPath(Span *inst, i32 childrenIdx, StrVec *path, void *value, word op);
Table *Seel_GetOrMake(Table *tbl, void *key, word op, i32 childrenIdx, cls typeOf);
