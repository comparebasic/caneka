typedef struct chain {
    Type type;
    struct lookup *funcs;
    struct chain *next;
} Chain;

Chain *Chain_Make(MemCtx *m);
status Chain_Extend(MemCtx *m, Chain *chain, Lookup *funcs);
void *Chain_Get(Chain *chain, word type);
