typedef struct chain {
    Type type;
    struct lookup *funcs;
    struct chain *next;
} Chain;

Chain *Chain_Make(struct mem_ctx *m, struct lookup *funcs);
status Chain_Extend(struct mem_ctx *m, Chain *chain, struct lookup *funcs);
void *Chain_Get(Chain *chain, word type);
