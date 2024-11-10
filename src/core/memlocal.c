#include <external.h>
#include <caneka.h>

void *MemLocal_GetPtr(LocalPtr *lptr){
    return NULL;
}

LocalPtr *MemLocal_GetLocalPtr(MemCtx *m, void *ptr){
    return NULL;
}

MemCtx *MemLocal_Make(){
    MemCtx *m = MemCtx_Make();
    m->type.state |= LOCAL_PTR;
    return m;
}
