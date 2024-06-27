#include <external.h>
#include <filestore.h>

void *Chain_Get(Chain *chain, word type){
    Virtual *t = Lookup_Get(chain->funcs, type);
    if(t != NULL){
       return (void *)t; 
    }
    if(chain->next != NULL){
        return Chain_Get(chain->next, type);
    }else{
        return NULL;
    }
}

status Chain_Extend(MemCtx *m, Chain *chain, Lookup *funcs){
    Chain *last = chain;
    while(last->next != NULL){
        last = last->next;
    }
    last->next = Chain_Make(m, funcs);
    return SUCCESS;
}

Chain *Chain_Make(MemCtx *m, Lookup *funcs){
    Chain *chain = (Chain *)MemCtx_Alloc(m, sizeof(Chain));
    chain->type.of = TYPE_CHAIN;
    chain->funcs = funcs;
    return chain;
}
