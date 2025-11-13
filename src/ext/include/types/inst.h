i32 Inst_Set(Span *inst, Table *seal, void *key, void *value);
Span *Inst_Make(MemCh *m, cls typeOf);
status Inst_ShowKeys(Buff *bf, Inst *inst, i32 childrenIdx, i32 indent);
void *Inst_ByPath(Span *inst, i32 childrenIdx, StrVec *path, void *value, word op);
Table *Inst_GetOrMake(Table *tbl, void *key, word op, i32 childrenIdx, cls typeOf);
