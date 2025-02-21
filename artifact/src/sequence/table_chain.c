#include <external.h>
#include <caneka.h>

void *TableChain_Get(TableChain *chain, Abstract *a){
    DebugStack_Push(a, a->type.of);
    if(chain->tbl == NULL){
        DebugStack_Pop();
        return NULL;
    }
    Abstract *t = Table_Get(chain->tbl, (Abstract *)a);

    if(t != NULL){
        DebugStack_Pop();
        return (void *)t; 
    }
    if(chain->next != NULL){
        Abstract *_a = TableChain_Get(chain->next, a);
        DebugStack_Pop();
        return _a;
    }else{
        DebugStack_Pop();
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
