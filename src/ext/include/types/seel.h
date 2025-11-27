extern struct lookup *SeelLookup;
extern struct lookup *SeelOrdLookup;
extern struct lookup *SeelNameLookup;
extern struct lookup *SeelChildrenPropLookup;

typedef cls seelType;

i32 Seel_GetIdx(Table *seel, void *key);
status Seel_Seel(MemCh *m, Table *seel, Str *name, cls typeOf, i32 childrenIdx);
Span *Seel_OrdSeel(MemCh *m, seelType instOf);
Table *Seel_GetSeel(MemCh *m, cls instType);
status Seel_Init(MemCh *m);
void *Seel_Get(Span *inst, void *key);
status Seel_Set(Span *inst, void *key, void *value);
status Seel_SetKv(Span *inst, Str *prop, void *key, void *value);
