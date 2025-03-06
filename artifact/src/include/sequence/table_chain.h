typedef struct table_chain {
    Type type;
    Span *tbl;
    struct table_chain *next;
} TableChain;
void *TableChain_Get(TableChain *chain, Abstract *a);
status TableChain_Extend(struct span *m, TableChain *chain, struct span *tbl);
TableChain *TableChain_Make(struct span *m, struct span *tbl);
