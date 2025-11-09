typedef Object NodeObj;

enum nodeobj_flags {
    NODEOBJ_CHILD_SPAN = 1 << 8,
};

enum nodeobj_prop_idx {
    NODEOBJ_PROPIDX_NAME = 0,
    NODEOBJ_PROPIDX_ATTS = 1,
    NODEOBJ_PROPIDX_VALUE = 2,
};

status NodeObj_ClsInit(MemCh *m);
void *NodeObj_Att(NodeObj *nobj, void *key);
