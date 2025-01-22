#include <external.h>
#include <caneka.h>

void *TableChain_Get(TableChain *chain, Abstract *a){
    if(chain->tbl == NULL){
        return NULL;
    }
    Abstract *t = Table_Get(chain->tbl, (Abstract *)a);

    if(t != NULL){
       return (void *)t; 
    }
    if(chain->next != NULL){
        return TableChain_Get(chain->next, a);
    }else{
        return NULL;
    }
}

status TableChain_Extend(MemCtx *m, TableChain *chain, Span *tbl){
    TableChain *last = chain;
    while(last->next != NULL){
        last = last->next;
    }
    if(last->tbl == NULL){
        last->tbl = tbl;
    }else{
        last->next = TableChain_Make(m, tbl);
    }
    return SUCCESS;
}

TableChain *TableChain_Make(MemCtx *m, Span *tbl){
    TableChain *chain = (TableChain *)MemCtx_Alloc(m, sizeof(TableChain));
    chain->type.of = TYPE_TABLE_CHAIN;
    chain->tbl = tbl;
    return chain;
}
