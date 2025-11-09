enum relation_flags {
    RELATION_ROW_START = 1 << 8,
    RELATION_ROW_END = 1 << 9,
};

typedef struct relation {
    Type type;
    i16 _;
    i16 stride;
    void **headers;
    Iter it;
} Relation;

Relation *Relation_Make(MemCh *m, i16 stride, void **headers);
status Relation_HeadFromValues(Relation *rel);
status Relation_Start(Relation *rel);
status Relation_Next(Relation *rel);
status Relation_SetValue(Relation *rel, i16 row, i16 col, void *value);
status Relation_AddValue(Relation *rel, void *value);
status Relation_RowCount(Relation *rel);
status Relation_ResetIter(Relation *rel);
