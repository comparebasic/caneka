typedef struct chain {
    Type type;
    struct lookup *funcs;
    struct chain *next;
} Chain;


Chain *Chain_Make(struct span *m, struct lookup *funcs);
status Chain_Extend(struct span *m, Chain *chain, struct lookup *funcs);
void *Chain_Get(Chain *chain, word type);
void *Chain_GetIfc(Chain *chain, word type);

