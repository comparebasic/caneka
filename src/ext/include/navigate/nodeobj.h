typedef Table NodeObj;

enum nodeobj_flags {
    NODEOBJ_CHILD_SPAN = 1 << 8,
};

status NodeObj_ClsInit(MemCh *m);
void *NodeObj_Att(Inst *nobj, void *key);
status NodeObj_SetAtt(Inst *nobj, void *key, void *value);
void *NodeObj_GetChild(Inst *nobj, void *key);
Table *NodeTable_Make(MemCh *m);
Table *NodeObj_GetOrMake(Table *tbl, void *key, word op);
Table *NodeObj_GetTblOfAtt(Inst *nobj, void *key);
