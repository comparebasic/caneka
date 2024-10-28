typedef struct table_chain {
    Type type;
    Span *tbl;
    struct chain *next;
} Chain;
void *TableChain_Get(Chain *chain, String *s);
status TableChain_Extend(struct mem_ctx *m, Chain *chain, struct span *tbl);
Chain *TableChain_Make(struct mem_ctx *m, struct span *tbl);
