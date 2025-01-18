typedef struct table_chain {
    Type type;
    Span *tbl;
    struct table_chain *next;
} TableChain;
void *TableChain_Get(TableChain *chain, Abstract *a);
status TableChain_Extend(struct mem_ctx *m, TableChain *chain, struct span *tbl);
TableChain *TableChain_Make(struct mem_ctx *m, struct span *tbl);
