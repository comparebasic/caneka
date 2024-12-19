#include <external.h>
#include <caneka.h>

void *TableChain_Get(TableChain *chain, String *s){

    Abstract *t = Table_Get(chain->tbl, (Abstract *)s);
    Debug_Print((void *)s, 0, "Table Chain Get s: ", COLOR_PURPLE, TRUE);
    printf("\n");
    Debug_Print((void *)chain->tbl, 0, "Table Chain Get: ", COLOR_PURPLE, TRUE);
    printf("\n");
    Debug_Print((void *)t, 0, "Table Chain Get t: ", COLOR_PURPLE, TRUE);
    printf("\n");

    if(t != NULL){
       return (void *)t; 
    }
    if(chain->next != NULL){
        return TableChain_Get(chain->next, s);
    }else{
        return NULL;
    }
}

status TableChain_Extend(MemCtx *m, TableChain *chain, Span *tbl){
    TableChain *last = chain;
    while(last->next != NULL){
        last = last->next;
    }
    last->next = TableChain_Make(m, tbl);
    return SUCCESS;
}

TableChain *TableChain_Make(MemCtx *m, Span *tbl){
    TableChain *chain = (TableChain *)MemCtx_Alloc(m, sizeof(TableChain));
    chain->type.of = TYPE_TABLE_CHAIN;
    chain->tbl = tbl;
    return chain;
}
