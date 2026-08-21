NGram *LatinTree_Make(MemCh *m, Buff *bf);
void LatinTree_Add(MemCh *m, NGram *ng, Str *s, i32 value);
i32 LatinTree_Find(MemCh *m, NGram *ng, Str *s, i32 value);
void LatinTree_Query(MemCh *m, NGram *ng, Str *s, i32 value);
