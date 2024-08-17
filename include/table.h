#define TABLE_MAX_DIMS 5
Abstract *Table_Get(Span *tbl, Abstract *a);
status Table_SetValue(Span *tbl, Abstract *a);
status Table_SetKey(Span *tbl, Abstract *a);
Hashed *Table_GetHashed(Span *tbl, Abstract *a);
int Table_GetIdx(Span *tbl, Abstract *a);
int Table_Set(Span *tbl, Abstract *a, Abstract *value);
Span *Table_Make(MemCtx *m);
