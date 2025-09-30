typedef struct persist_item {
    Type type;
    void *ptr;
    PersistCoord coord;
} PersistItem;

PersistItem *PersistItem_Make(MemCh *m, i32 slIdx, void *ptr, cls typeOf);
