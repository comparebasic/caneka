typedef struct ordtable {
    Type type;
    Table *tbl;
    Span *order;
} OrdTable;

OrdTable *OrdTable_Make(MemCh *m);
Hashed *OrdTable_Set(OrdTable *otbl, Abstract *key, Abstract *value);
Hashed *OrdTable_Get(OrdTable *otbl, Abstract *key);
Hashed *OrdTable_GetByIdx(OrdTable *otbl, i32 idx);
boolean OrdTable_IsBlank(OrdTable *otbl);
OrdTable *OrdTable_GetOrMake(OrdTable *otbl, Abstract *key);
status OrdTable_AddByPath(OrdTable *otbl, StrVec *path, Abstract *value);
