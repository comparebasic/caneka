typedef struct persisted_list {
    Type type;
    Span *list;
    Span *tbl;
    File *file;
} PersistedList;
