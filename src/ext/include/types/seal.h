extern struct lookup *SealLookup;
extern struct lookup *SealNameLookup;

i32 Seal_GetIdx(Table *seal, void *key);
status Seal_Seal(MemCh *m, Table *seal, Str *name, cls typeOf);
status Seal_Init(MemCh *m);
