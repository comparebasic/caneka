#define MEM_PERSIST_MASK 4095

typedef struct ref_coords {
    cls typeOf;
    i16 idx;
    quad offset;
} PersistCoord;

typedef struct persist_item {
    Type type;
    void *ptr;
    PersistCoord coord;
} PersistItem;

typedef status (*PersistFunc)(Stream *sm, byte[MEM_SLAB_SIZE], Abstract *source);

extern struct lookup *BlankerLookup;
extern struct lookup *RepointerLookup;

status Persist_Init(MemCh *m);
cls Persist_RepointAddr(MemCh *pm, void **ptr);
status Persist_FlushFree(Stream *sm, MemCh *persist);
status Persist_FromStream(MemCh *m, Stream *sm);

PersistItem *PersistItem_Make(MemCh *m, i32 slIdx, void *ptr, cls typeOf);
void PersistCoord_Fill(PersistCoord *coord, i32 slIdx, void *ptr, cls typeOf);
