typedef Table NodeObj;

enum nodeobj_flags {
    NODEOBJ_CHILD_SPAN = 1 << 8,
};

status NodeObj_ClsInit(MemCh *m);
Table *NodeTable_Make(MemCh *m);
Table *NodeObj_GetOrMake(Table *tbl, void *key, word op);
