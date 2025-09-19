#define MEM_PERSIST_MASK 4095 
#define PERSIST_ARR_MAX ((MEM_SLAB_SIZE-sizeof(MemCh)-sizeof(MemPage)-sizeof(Persist)) \
    / sizeof(PersistItem))

typedef struct ref_coords {
    cls typeOf;
    i16 idx;
    quad offset;
} PersistCoord;

typedef struct persist_item {
    void *ptr;
    PersistCoord coord;
} PersistItem;

typedef struct persist {
    Type type;
    i16 count;
    i32 maxIdx;
} Persist;

typedef status (*PersistFunc)(Stream *sm, byte[MEM_SLAB_SIZE], Abstract *source);

extern struct lookup *BlankerLookup;
extern struct lookup *RepointerLookup;

Persist *Persist_Get(MemCh *m);
PersistItem *Persist_GetArray(MemCh *m);
status Persist_SetRef(MemCh *m, i32 slIdx, MemPage *pg, void *ptr);
status Persist_FillRef(MemCh *m, void **ptr);
MemCh *Persist_Make();
status Persist_Flush();
