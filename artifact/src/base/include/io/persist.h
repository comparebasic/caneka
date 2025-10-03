#define MEM_PERSIST_MASK 4095

typedef status (*PersistFunc)(Stream *sm, byte[MEM_SLAB_SIZE], Abstract *source);

extern struct lookup *BlankerLookup;
extern struct lookup *RepointerLookup;

status Persist_Init(MemCh *m);
cls Persist_RepointAddr(MemCh *pm, void **ptr);
status Persist_FlushFree(Stream *sm, MemCh *persist);
MemCh *Persist_FromStream(Stream *sm);
status Persist_PackAddr(cls typeOf, i32 slIdx, void **ptr);
cls Persist_UnpackAddr(PersistCoord *coord, Abstract **arr);
