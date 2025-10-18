#define MEM_STASH_MASK 4095

typedef status (*StashFunc)(Stream *sm, byte[MEM_SLAB_SIZE], Abstract *source);

extern struct lookup *BlankerLookup;
extern struct lookup *RepointerLookup;

status Stash_Init(MemCh *m);
cls Stash_RepointAddr(MemCh *pm, void **ptr);
status Stash_FlushFree(Stream *sm, MemCh *persist);
MemCh *Stash_FromStream(Stream *sm);
status Stash_PackAddr(cls typeOf, i32 slIdx, void **ptr);
cls Stash_UnpackAddr(MemCh *m, StashCoord *coord, Abstract **arr);
