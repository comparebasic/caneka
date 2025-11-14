typedef Table NodeObj;

enum nodeobj_flags {
    NODEOBJ_CHILD_SPAN = 1 << 8,
};

enum nodeobj_prop_idx {
    NODEOBJ_PROPIDX_NAME = 0,
    NODEOBJ_PROPIDX_ATTS = 1,
    NODEOBJ_PROPIDX_CHILDREN = 2,
};

status NodeObj_ClsInit(MemCh *m);
void *NodeObj_Att(Inst *nobj, void *key);
Table *NodeTable_Make(MemCh *m);
void *NodeObj_ByPath(Span *inst, StrVec *path, void *value, word op);
Table *NodeObj_GetOrMake(Table *tbl, void *key, word op);
