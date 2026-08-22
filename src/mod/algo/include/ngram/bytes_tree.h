NGram *BytesTree_Make(MemCh *m, Buff *bf);
void BytesTree_Add(MemCh *m, NGram *ng, Str *s, util meaning);
void BytesTree_Query(MemCh *m, NGram *ng, Str *s, util meaning);
util BytesTree_Find(MemCh *m, NGram *ng, Str *s);
