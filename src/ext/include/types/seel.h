extern struct lookup *SeelLookup;
extern struct lookup *SeelNameLookup;

i32 Seel_GetIdx(Table *seel, void *key);
status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf, i32 childrenIdx);
status Seel_Init(MemCh *m);
void *Seel_Get(Span *inst, void *key);
status Seel_Set(Span *inst, void *key, void *value);
