status Table_Underlay(Table *a, Table *b);
status Table_Overlay(Table *a, Table *b);
status Table_Lay(Span *dest, Span *src, boolean overlay);
status Table_Merge(Table *dest, Table *src);
Table *Table_GetOrMake(Table *tbl, void *key, word op);
void *Table_ByPath(Table *tbl, StrVec *path, void *value, word op);
Span *Table_Ordered(MemCh *m, Table *tbl);
