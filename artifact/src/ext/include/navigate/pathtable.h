typedef struct ordtable {
    Type type;
    Type instType;
    ClassDef *cls;
    Table *tbl;
    Span *order;
} Object;

PathTable *PathTable_Make(MemCh *m);
Hashed *PathTable_Set(PathTable *otbl, Abstract *key, Abstract *value);
Hashed *PathTable_Get(PathTable *otbl, Abstract *key);
Hashed *PathTable_GetByIdx(PathTable *otbl, i32 idx);
boolean PathTable_IsBlank(PathTable *otbl);
PathTable *PathTable_GetOrMake(PathTable *otbl, Abstract *key);
status PathTable_AddByPath(PathTable *otbl, StrVec *path, Abstract *value);
