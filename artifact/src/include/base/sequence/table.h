typedef Span Table;

typedef struct hkey {
   Type type; 
   i32 idx;
   util id;
   i8 dim;
   i8 pos;
} HKey;

typedef Pair Collision; 

status HKey_Init(HKey *hk, Table *tbl, util id);
inline status Table_HKeyVal(Table *tbl, HKey *hk);
status Table_SetKey(Iter *it, Abstract *a);
i32 Table_SetIdxEntry(Iter *it, Abstract *a);
Hashed *Table_SetValue(Iter *it, Abstract *a);
Hashed *Table_GetHashed(Span *tbl, Abstract *a);
Hashed *Table_SetHashed(Span *tbl, Abstract *a, Abstract *value);
Abstract *Table_GetKey(Span *tbl, i32 idx);
Abstract *Table_Get(Span *tbl, Abstract *a);
int Table_Set(Span *tbl, Abstract *a, Abstract *value);
Abstract *Table_FromIdx(Span *tbl, int idx);
int Table_GetIdx(Span *tbl, Abstract *a);
Table *Table_Make(MemCh *m);
