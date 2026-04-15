typedef struct persist_item {
    Type type;
    void *ptr;
    StashCoord coord;
} StashItem;

StashItem *StashItem_Make(MemCh *m, i32 slIdx, void *ptr, cls typeOf);
