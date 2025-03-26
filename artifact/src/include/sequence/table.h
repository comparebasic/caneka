/*
Table
The hash based key-value lookup structure, built on top of the Span Data Structure.
*/
Abstract *Table_Get(Span *tbl, Abstract *a);
Abstract *Table_FromIdx(Span *tbl, int idx);
Hashed *Table_SetValue(Iter *it, Abstract *a);
status Table_SetKey(Iter *it, Abstract *a);
Hashed *Table_GetHashed(Span *tbl, Abstract *a);
Hashed *Table_SetHashed(Span *tbl, Abstract *a, Abstract *value);
i32 Table_GetIdx(Span *tbl, Abstract *a);
i32 Table_Set(Span *tbl, Abstract *a, Abstract *value);
Span *Table_Make(MemCh *m);
status Table_Merge(Span *tbl, Span *oldTbl);
i32 Table_SetIdxEntry(Iter *it, Abstract *a);
Abstract *Table_GetKey(Span *tbl, i32 idx);
status Table_SetRaw(Span *tbl, Str *key, util *u);
util Table_GetRaw(Span *tbl, Str *key);
