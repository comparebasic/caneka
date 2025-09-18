typedef struct persist {
    Type type;
    quad total;
}Persist;

typedef status (*PersistFunc)(Stream *sm, byte[MEM_SLAB_SIZE], Abstract *source);

extern struct lookup *BlankerLookup;
extern struct lookup *RepointerLookup;

struct span *Persist_GetTable(MemCh *m);
status Persist_SetRef(MemCh *m, i32 slIdx, MemPage *pg, void *ptr);
MemCh *Persist_Make();
status Persist_Flush();
