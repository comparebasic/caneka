/*
Table
The hash based key-value lookup structure, built on top of the Span Data Structure.

related: core/table.c
related: include/span.h
*/
#define TABLE_MAX_DIMS 5
Abstract *Table_Get(Span *tbl, Abstract *a);
Abstract *Table_FromIdx(Span *tbl, int idx);
Hashed *Table_SetValue(Span *tbl, Abstract *a);
status Table_SetKey(Span *tbl, Abstract *a);
Hashed *Table_GetHashed(Span *tbl, Abstract *a);
Hashed *Table_SetHashed(Span *tbl, Abstract *a, Abstract *value);
i32 Table_GetIdx(Span *tbl, Abstract *a);
i32 Table_Set(Span *tbl, Abstract *a, Abstract *value);
Span *Table_Make(MemCtx *m);
status Table_Merge(Span *tbl, Span *oldTbl);
i32 Table_SetIdxEntry(Span *tbl, Abstract *a);
Abstract *Table_GetKey(Span *tbl, Abstract *a);
