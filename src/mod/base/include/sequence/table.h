typedef Span Table;

enum table_flags {
    TABLE_SEALED = 1 << 8,
};

typedef struct hkey {
   Type type; 
   i32 idx;
   util id;
   i8 dim;
   i8 pos;
} HKey;

status Table_SetKey(Iter *it, void *a);
i32 Table_SetIdxEntry(Iter *it, void *a);
Hashed *Table_SetValue(Iter *it, void *a);
Hashed *Table_GetHashed(Span *tbl, void *a);
Hashed *Table_SetHashed(Span *tbl, void *a, void *value);
void *Table_GetKey(Span *tbl, i32 idx);
void *Table_Get(Span *tbl, void *a);
i32 Table_UnSet(Table *tbl, void *a);
i32 Table_Set(Span *tbl, void *a, void *value);
i32 Table_SetByIter(Iter *it, void *a, void *value);
void *Table_FromIdx(Span *tbl, i32 idx);
i32 Table_GetIdx(Span *tbl, void *a);
Table *Table_Make(MemCh *m);

status Table_HKeyVal(HKey *hk);
status Table_HKeyMiss(HKey *hk);
status Table_HKeyInit(HKey *hk, i8 dims, util id);
