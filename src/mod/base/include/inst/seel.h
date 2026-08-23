extern struct lookup *SeelLookup;
extern struct lookup *SeelOrdLookup;
extern struct lookup *SeelNameLookup;
extern struct span *SeelByName;

typedef cls seelType;

i32 Seel_GetIdx(struct span *seel, void *key);
status Seel_Seel(struct mem_ctx *m, struct span *seel, struct str *name, cls typeOf);
struct span *Seel_OrdSeel(struct mem_ctx *m, seelType instOf);
struct span *Seel_GetSeel(struct mem_ctx *m, cls instType);
status Seel_Init(struct mem_ctx *m);
void *Seel_Get(struct span *inst, void *key);
status Seel_Set(struct span *inst, void *key, void *value);
status Seel_AddTo(struct span *inst, void *attKey, void *key, void *value);
i32 Seel_TypeByName(void *name);
