typedef struct ordtable {
    Type type;
    Table *tbl;
    Span *order;
} OrdTable;

OrdTable *OrdTable_Make(MemCh *m);
Hashed *OrdTable_Set(OrdTable *otbl, Abstract *key, Abstract *value);
Hashed *OrdTable_Get(OrdTable *tbl, Abstract *key);
Hashed *OrdTable_GetByIdx(OrdTable *tbl, i32 idx);
